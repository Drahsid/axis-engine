#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <ultra64.h>
#include <string.h>
#include "dma_object.h"
#include "stdint.h"
#include "printf.h"

// TODO: move these string funcs
int strcmp(const char* p1, const char* p2)
{
    const unsigned char* s1 = (const unsigned char*)p1;
    const unsigned char* s2 = (const unsigned char*)p2;
    unsigned char c1, c2;
    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0') return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

char* strcpy(char* dest, const char* src)
{
    return memcpy(dest, src, strlen(src) + 1);
}

// TODO: move this into a shared header
uint64_t djb2_hash(const char* input) {
    int index;
    uint64_t hash = 1337420;

    for (index = 0; input[index] != '\0'; index++) {
        hash = ((hash << 5) + hash) + input[index];
    }

    return hash;
}

const char* g_filesystem_good_header = "hash_fs";

typedef struct
{
    dma_object_t dma_obj;
    char header[8];
    uint64_t num_files __attribute__((aligned (8)));
    uint64_t archive_size __attribute__((aligned (8)));
    uint32_t fs_addr;
    uint32_t file_offsets;
    uint32_t file_hashes;
    uint32_t archive;
} filesystem_info_t __attribute__((aligned (16)));

void filesystem_info_construct(filesystem_info_t* filesystem, OSPiHandle* handler, char* segment) {
    uint8_t read_in_data[8] __attribute__((aligned (8)));
    uint32_t i = 0;
    uint32_t q = 0;

    filesystem->fs_addr = NULL;
    strcpy(filesystem->header, "BADBAD!");
    filesystem->num_files = 0;
    filesystem->file_offsets = NULL;
    filesystem->file_hashes = NULL;
    filesystem->archive_size = 0;
    filesystem->archive = NULL;

    dma_object_construct(&filesystem->dma_obj, handler, segment);

    osInvalDCache(filesystem, sizeof(filesystem));

    // TODO: Find from segment, this is BAD
    for (i = 0x1500; filesystem->fs_addr == 0; i += 8) {
        //TODO: wrap this into a dma_object function
        dma_object_read_rom(&filesystem->dma_obj, i, read_in_data, 8);

        if (strcmp(read_in_data, g_filesystem_good_header) == 0) {
            if (q == 0) {
                q++;
            }
            else {
                filesystem->fs_addr = i;
                printf("Found filesystem at %X\n", i);
                break;
            }
        }
    }

    if (filesystem->fs_addr == NULL) {
        printf("error: failed to find filesystem in rom!\n");
        for(;;);
    }

    // 0x00 -> 0x08 = header
    strcpy(filesystem->header, read_in_data);
    i = filesystem->fs_addr + 0xCu; // 0x08 -> 0x0C = pad, so we skip it

    dma_object_read_rom(&filesystem->dma_obj, i, &filesystem->num_files, sizeof(uint64_t));
    i += sizeof(uint64_t); // 0x0C -> 0x14 = num files

    filesystem->file_offsets = i;
    i += (sizeof(uint32_t) * filesystem->num_files); // file offsets are 4 bytes each

    filesystem->file_hashes = i;
    i += (sizeof(uint64_t) * filesystem->num_files) + 4; // file hashes are 8 bytes each, then there is 4 bytes of padding

    filesystem->archive = (i + sizeof(uint64_t)); // the archive size is stored in 8 bytes
    dma_object_read_rom(&filesystem->dma_obj, i, &filesystem->archive_size, 8);

    printf("Filesystem info\nheader: %s, num files: %llu, offsets address: %X, hashes address: %X, size: %llX, data address: %X\n",
        filesystem->header, filesystem->num_files, filesystem->file_offsets, filesystem->file_hashes, filesystem->archive_size, filesystem->archive);
}

// index == num_files on fail
uint32_t filesystem_info_get_index_from_hash(uint64_t hash, filesystem_info_t* filesystem) {
    uint32_t index = 0;
    uint64_t rhash __attribute__((aligned (8))) = 0;

    // TODO: malloc room for offsets and hashes and store them in rdram upon construction of filesystem_info_t
    for (index = 0; index < filesystem->num_files; index++) {
        dma_object_read_rom(&filesystem->dma_obj, filesystem->file_hashes + (index * sizeof(uint64_t)), &rhash, sizeof(uint64_t));
        if (rhash == hash) return index;
    }

    return index;
}

uint32_t filesystem_info_get_offset_from_index(uint32_t index, filesystem_info_t* filesystem) {
    uint32_t offset __attribute__((aligned (8))) = 0;

    dma_object_read_rom(&filesystem->dma_obj, filesystem->file_offsets + (index * sizeof(uint32_t)), &offset, sizeof(uint32_t));

    return offset;
}

void* filesystem_info_read_file(const char* file, void* dest, filesystem_info_t* filesystem) {
    uint64_t file_size = 0;
    uint64_t hash = djb2_hash(file);
    uint32_t index = filesystem_info_get_index_from_hash(hash, filesystem);
    uint32_t offset = 0;

    if (index < filesystem->num_files) {
        if (index == (filesystem->num_files - 1)) {
            offset = filesystem_info_get_offset_from_index(index, filesystem);
            file_size = filesystem->archive_size - offset;
        }
        else {
            offset = filesystem_info_get_offset_from_index(index, filesystem);
            file_size = filesystem_info_get_offset_from_index(index + 1, filesystem) - offset;
        }

        osInvalDCache(dest, file_size);
        dma_object_read_rom(&filesystem->dma_obj, filesystem->archive + offset, dest, file_size);
    }
    else {
        printf("File %s not found!\n", file);
        return NULL;
    }
}

#endif
