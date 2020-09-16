#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    size_t file_num;
    size_t* file_offsets;
    uint64_t* file_hashes;
    char** file_paths;
} file_table_info_t;

typedef struct
{
    char header[8];
    size_t archive_size;
    file_table_info_t file_table_info;
    void* archive_buffer;
} archive_data_t;

void file_table_info_t_construct(file_table_info_t* file_table_info) {
    file_table_info->file_num = 0;
    file_table_info->file_offsets = malloc(sizeof(size_t*));
    file_table_info->file_hashes = malloc(sizeof(uint64_t*));
    file_table_info->file_paths = malloc(sizeof(char**));
}

void archive_data_t_construct(archive_data_t* archive_data) {
    strcpy(archive_data->header, "hash_fs\0");
    archive_data->archive_size = 0;
    file_table_info_t_construct(&archive_data->file_table_info);
    archive_data->archive_buffer = malloc(sizeof(int));
}

void archive_data_t_write_to_file(char* out_file, archive_data_t* data) {
    FILE* file;
    int i = 0;

    file = fopen(out_file, "w+");

    if (file == NULL) {
        printf("error opening file for writing %s\n", out_file);
        for(;;);
    }

    printf("writing header %s, %llu files with an archive size %llX\n\n", data->header, data->file_table_info.file_num, data->archive_size);


    for (i = 0; i < data->file_table_info.file_num; i++) {
        printf("file %s has offset %llX\n", data->file_table_info.file_paths[i], data->file_table_info.file_offsets[i]);
        printf("file %s has hash %llX\n", data->file_table_info.file_paths[i], data->file_table_info.file_hashes[i]);
    }

    printf("archive data: \n");

    for (i = 0; i < data->archive_size; i++) {
        printf("%X ", ((uint8_t*)data->archive_buffer)[i]);
    }

    printf("\n");


    //fprintf(file, data->header);
    fwrite(data->header, sizeof(uint8_t), 8, file);
    for (i = 0; i < 4; i++) fputc(0, file);

    size_t file_num_bswap = __builtin_bswap64(data->file_table_info.file_num);
    size_t archive_size_bswap = __builtin_bswap64(data->archive_size);

    fwrite(&file_num_bswap, sizeof(size_t), 1, file);

    for (i = 0; i < data->file_table_info.file_num; i++) {
        data->file_table_info.file_offsets[i] = __builtin_bswap64(data->file_table_info.file_offsets[i]);
        fwrite(&data->file_table_info.file_offsets[i], sizeof(size_t), 1, file);
    }

    for (i = 0; i < data->file_table_info.file_num; i++) {
        data->file_table_info.file_hashes[i] = __builtin_bswap64(data->file_table_info.file_hashes[i]);
        fwrite(&data->file_table_info.file_hashes[i], sizeof(uint64_t), 1, file);
    }

    //fwrite(data->file_table_info.file_offsets, sizeof(size_t), data->file_table_info.file_num, file);
    //fwrite(data->file_table_info.file_hashes, sizeof(uint64_t), data->file_table_info.file_num, file);

    for (i = 0; i < 4; i++) fputc(0, file);

    fwrite(&archive_size_bswap, sizeof(size_t), 1, file);
    fwrite(data->archive_buffer, sizeof(uint8_t), data->archive_size, file);

    fclose(file);
}

#endif

