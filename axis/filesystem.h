#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <ultra64.h>
#include <string.h>
#include "dma_object.h"
#include "heap.h"
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
    char header[8] __attribute__((aligned(8)));
    uint64_t num_files __attribute__((aligned(8)));
    uint64_t archive_size __attribute__((aligned(8)));
    uint32_t fs_addr __attribute__((aligned(8)));
    uint32_t archive;
    uint32_t* file_offsets;
    uint64_t* file_hashes;
} filesystem_info_t __attribute__((aligned(16)));

void filesystem_info_construct(filesystem_info_t* filesystem, OSPiHandle* handler, char* segment0, char* segment1) {
    uint32_t offset_addr = 0;

    filesystem->fs_addr = NULL;
    strcpy(filesystem->header, "BADBAD!");
    filesystem->num_files = 0;
    filesystem->file_offsets = NULL;
    filesystem->file_hashes = NULL;
    filesystem->archive_size = 0;
    filesystem->archive = NULL;

    dma_object_construct(&filesystem->dma_obj, handler, segment0);

    osInvalDCache(filesystem, sizeof(filesystem));
    filesystem->fs_addr = segment1;

    // 0x00 -> 0x08 = header
    dma_object_read_rom(&filesystem->dma_obj, filesystem->fs_addr, filesystem->header, 8);
    offset_addr = filesystem->fs_addr + 0xCu; // 0x08 -> 0x0C = pad, so we skip it

    dma_object_read_rom(&filesystem->dma_obj, offset_addr, &filesystem->num_files, sizeof(uint64_t));
    offset_addr += sizeof(uint64_t); // 0x0C -> 0x14 = num files

    // file offsets are 4 bytes each
    filesystem->file_offsets = malloc(sizeof(uint32_t) * filesystem->num_files + HEAP_BLOCK_ALIGNMENT);
    dma_object_read_rom(&filesystem->dma_obj, offset_addr, filesystem->file_offsets, sizeof(uint32_t) * filesystem->num_files);
    offset_addr += (sizeof(uint32_t) * filesystem->num_files);

    // file hashes are 8 bytes each, then there is 4 bytes of padding
    filesystem->file_hashes = malloc(sizeof(uint64_t) * filesystem->num_files + HEAP_BLOCK_ALIGNMENT);
    dma_object_read_rom(&filesystem->dma_obj, offset_addr, filesystem->file_hashes, sizeof(uint64_t) * filesystem->num_files);
    offset_addr += (sizeof(uint64_t) * filesystem->num_files) + 4;

    filesystem->archive = (offset_addr + sizeof(uint64_t)); // the archive size is stored in 8 bytes
    dma_object_read_rom(&filesystem->dma_obj, offset_addr, &filesystem->archive_size, 8);

    printf("\nFilesystem info\nheader: %s, num files: %llu, size: %llX, data address: %X\n", filesystem->header, filesystem->num_files, filesystem->archive_size, filesystem->archive);
}

// index == num_files on fail
uint32_t filesystem_info_get_index_from_hash(filesystem_info_t* filesystem, uint64_t hash) {
    uint32_t index;

    for (index = 0; index < filesystem->num_files; index++) {
        if (filesystem->file_hashes[index] == hash) return index;
    }

    return index;
}

// calling this wastes more horizontal space than simply filesystem->file_offsets[index]
uint32_t filesystem_info_get_offset_from_index(filesystem_info_t* filesystem, uint32_t index) {
    return filesystem->file_offsets[index];
}

// TODO: might be able to clean repeated code up
uint32_t filesystem_info_get_file_size(filesystem_info_t* filesystem, const char* file) {
    uint64_t hash = djb2_hash(file);
    uint32_t index = filesystem_info_get_index_from_hash(filesystem, hash);
    uint32_t file_size = 0;
    uint32_t offset = 0;

    if (index < filesystem->num_files) {
        if (index == (filesystem->num_files - 1)) {
            offset = filesystem->file_offsets[index];
            file_size = filesystem->archive_size - offset;
        }
        else {
            offset = filesystem->file_offsets[index];
            file_size = filesystem->file_offsets[index + 1]; - offset;
        }
    }
    else {
        printf("File %s not found!\n", file);
        return NULL;
    }

    return file_size;
}

void filesystem_info_read_file(filesystem_info_t* filesystem, const char* file, void* dest) {
    uint64_t hash = djb2_hash(file);
    uint32_t index = filesystem_info_get_index_from_hash(filesystem, hash);
    uint32_t file_size = 0;
    uint32_t offset = 0;

    if (index < filesystem->num_files) {
        if (index == (filesystem->num_files - 1)) {
            offset = filesystem->file_offsets[index];
            file_size = filesystem->archive_size - offset;
        }
        else {
            offset = filesystem->file_offsets[index];
            file_size = filesystem->file_offsets[index + 1]; - offset;
        }

        osInvalDCache(dest, file_size);
        dma_object_read_rom(&filesystem->dma_obj, filesystem->archive + offset, dest, file_size);
    }
    else {
        printf("File %s not found!\n", file);
    }
}

void* filesystem_info_alloc_and_read_file(filesystem_info_t* filesystem, const char* file) {
    uint64_t hash = djb2_hash(file);
    uint32_t index = filesystem_info_get_index_from_hash(filesystem, hash);
    uint32_t file_size = 0;
    uint32_t offset = 0;
    void* dest = NULL;

    if (index < filesystem->num_files) {
        if (index == (filesystem->num_files - 1)) {
            offset = filesystem->file_offsets[index];
            file_size = filesystem->archive_size - offset;
        }
        else {
            offset = filesystem->file_offsets[index];
            file_size = filesystem->file_offsets[index + 1]; - offset;
        }

        dest = malloc(file_size);
        osInvalDCache(dest, file_size);
        dma_object_read_rom(&filesystem->dma_obj, filesystem->archive + offset, dest, file_size);
    }
    else {
        printf("File %s not found!\n", file);
        return NULL;
    }

    return dest;
}

#endif /* FILESYSTEM_H */

