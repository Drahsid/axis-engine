#ifndef DMAOBJ_H
#define DMAOBJ_J

#include <ultra64.h>
#include "stdint.h"

extern char _codeSegmentEnd[];
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];

typedef struct
{
    OSMesgQueue dma_message_queue;
    OSMesg dma_message_buffer;
    OSIoMesg dma_io_message_buffer;
} dma_object_t;

void dma_object_construct(dma_object_t* dma_obj, OSPiHandle* handler, char* segment) {
    osCreateMesgQueue(&dma_obj->dma_message_queue, &dma_obj->dma_message_buffer, 1);

    dma_obj->dma_io_message_buffer.hdr.pri = OS_MESG_PRI_NORMAL;
    dma_obj->dma_io_message_buffer.hdr.retQueue = &dma_obj->dma_message_queue;
    dma_obj->dma_io_message_buffer.dramAddr = segment;
    dma_obj->dma_io_message_buffer.devAddr = (uint32_t)_staticSegmentRomStart;
    dma_obj->dma_io_message_buffer.size = (uint32_t)_staticSegmentRomEnd - (uint32_t)_staticSegmentRomStart;

    osEPiStartDma(handler, &dma_obj->dma_io_message_buffer, OS_READ);
    osRecvMesg(&dma_obj->dma_message_queue, NULL, OS_MESG_BLOCK);
}

void dma_object_read_rom(dma_object_t* dma_obj, uint32_t src, void* dest, uint32_t length) {
    while (osPiStartDma(&dma_obj->dma_io_message_buffer, 1, OS_READ, src, dest, length, &dma_obj->dma_message_queue) == -1) {}
    osRecvMesg(&dma_obj->dma_message_queue, NULL, OS_MESG_BLOCK);
}

void dma_object_read_rom_with_pri(dma_object_t* dma_obj, uint32_t src, void* dest, uint32_t length, int priority) {
    while (osPiStartDma(&dma_obj->dma_io_message_buffer, priority, OS_READ, src, dest, length, &dma_obj->dma_message_queue) == -1) {}
    osRecvMesg(&dma_obj->dma_message_queue, NULL, OS_MESG_BLOCK);
}

#endif

