#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#define MAX_PATH 260
#endif

#include "filesystem.h"

size_t align_size(size_t lhs, size_t modulo)
{
    if (modulo == 0) return lhs;

    int remainder = abs(lhs) % modulo;
    if (remainder == 0) return lhs;

    return lhs + modulo - remainder;
}

uint64_t djb2_hash(const char* input) {
    int index;
    uint64_t hash = 1337420;

    for (index = 0; input[index] != '\0'; index++) {
        hash = ((hash << 5) + hash) + input[index];
    }

    return hash;
}

typedef struct {
    void* file_buffer;
} file_buffer_t;

int generate_data(char* full_dir, char* local_dir, archive_data_t* data, file_buffer_t* file_bufp) {
    struct dirent* dirfile;
    DIR* dir_ptr;
    FILE* file;
    int new_num;
    size_t file_size, initial_file_size;
    char new_dir[MAX_PATH];
    char new_file_dir[MAX_PATH];

    memset(new_dir, '\0', MAX_PATH);
    memset(new_file_dir, '\0', MAX_PATH);

    dir_ptr = opendir(full_dir);

    if (dir_ptr == NULL) {
        printf("error: failed to open directory %s\n", full_dir);
        return -1;
    }

    while (dirfile = readdir(dir_ptr)) {
        printf("NAME: %s\n", dirfile->d_name);
        int valid = strcmp(dirfile->d_name, ".") != 0 && strcmp(dirfile->d_name, "..") != 0;
        if (valid && dirfile->d_type != DT_DIR) {
            initial_file_size = 0;

            strcpy(new_dir, full_dir);
            strcat(new_dir, "/");
            strcat(new_dir, dirfile->d_name);

            strcpy(new_file_dir, local_dir);
            strcat(new_file_dir, "/");
            strcat(new_file_dir, dirfile->d_name);

            file = fopen(new_dir, "r");
            if (file != NULL) {
                printf("\nopened file %s (local: %s)\n",  new_dir, new_file_dir);

                fseek(file, 0, SEEK_END);
                initial_file_size = ftell(file);
                file_size = align_size(initial_file_size, 4);

                if (initial_file_size == -1) {
                    printf("error reading file %s!\n", new_dir);
                    for(;;);
                }

                if (fseek(file, 0, SEEK_SET) != 0) {
                    printf("error reading file %s!\n", new_dir);
                    for(;;);
                }

                file_bufp->file_buffer = realloc(file_bufp->file_buffer, file_size);
                memset(((uint8_t*)file_bufp->file_buffer) + initial_file_size, 0, file_size - initial_file_size);

                fread(file_bufp->file_buffer, sizeof(uint8_t), initial_file_size, file);
                fclose(file);

                printf("read file\n");

                new_num = data->file_table_info.file_num + 1;

                data->file_table_info.file_offsets = realloc(data->file_table_info.file_offsets, sizeof(size_t) * new_num);
                data->file_table_info.file_hashes = realloc(data->file_table_info.file_hashes, sizeof(uint64_t) * new_num);
                data->file_table_info.file_paths = realloc(data->file_table_info.file_paths, sizeof(char*) * new_num);

                data->file_table_info.file_offsets[data->file_table_info.file_num] = data->archive_size;
                data->file_table_info.file_hashes[data->file_table_info.file_num] = djb2_hash(new_file_dir);
                data->file_table_info.file_paths[data->file_table_info.file_num] = malloc(strlen(new_file_dir) + 1);
                strcpy(data->file_table_info.file_paths[data->file_table_info.file_num], new_file_dir);

                data->archive_buffer = realloc(data->archive_buffer, data->archive_size + file_size + 1);
                memcpy(((uint8_t*)data->archive_buffer) + data->archive_size, file_bufp->file_buffer, file_size);
                data->archive_size += file_size;

                data->file_table_info.file_num++;
                printf("done\n\n");
            }
            else {
                printf("Failed to open file: %s\n", new_file_dir);
                for(;;) {}
            }

            memset(new_dir, 0, MAX_PATH);
        }
        else if (valid && dirfile->d_type == DT_DIR) {
            strcpy(new_dir, full_dir);
            strcat(new_dir, "/");
            strcat(new_dir, dirfile->d_name);

            strcpy(new_file_dir, local_dir);
            strcat(new_file_dir, "/");
            strcat(new_file_dir, dirfile->d_name);

            printf("\ndelving into dir %s (local: %s)\n\n", new_dir, new_file_dir);
            generate_data(new_dir, new_file_dir, data, file_bufp);
        }
    }

    closedir(dir_ptr);
}

int main(int argc, char* argv[]) {
    printf("fspack started\n");
    archive_data_t data;
    file_buffer_t file_buf_object;
    char* directory = 0;
    char* out_file = 0;
    char local_dir[MAX_PATH];

    memset(local_dir, '\0', MAX_PATH);

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-D") == 0 || strcmp(argv[i], "--directory") == 0) {
            if (i + 1 != argc) {
                directory = argv[i + 1];
            }
        }

        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "-O") == 0 || strcmp(argv[i], "--outputdir") == 0) {
            if (i + 1 != argc) {
                out_file = argv[i + 1];
            }
        }
    }

    if (directory == 0) {
        printf("error: no input directory! use -d [dir] to choose a directory!\n");
        return 1;
    }

    if (out_file == 0) {
        printf("error: no output file! use -o [file] to choose a file!\n");
        return 1;
    }

    printf("using directory %s\n", directory);

    archive_data_t_construct(&data);
    file_buf_object.file_buffer = malloc(0x16000);

    generate_data(directory, local_dir, &data, &file_buf_object);
    archive_data_t_write_to_file(out_file, &data);

    printf("fspack finished\n");

    free(data.archive_buffer);

    for (int i = 0; i < data.file_table_info.file_num; i++) {
        free(data.file_table_info.file_paths[i]);
    }

    free(data.file_table_info.file_paths);
    free(data.file_table_info.file_hashes);
    free(data.file_table_info.file_offsets);
    free(file_buf_object.file_buffer);
    return 0;
}