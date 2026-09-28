/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "heap.h"
#include "os_config.h"
#include "cortex_m3.h"
#include <stddef.h>

typedef struct block {
    uint32_t      size;
    uint8_t       used;
    struct block *next;
} block_t;

#define BLOCK_HDR sizeof(block_t)

static uint8_t  heap_pool[HEAP_SIZE] __attribute__((aligned(4)));
static block_t *free_list;
static uint32_t total_used;

void os_heap_init(void) {
    free_list        = (block_t *)heap_pool;
    free_list->size  = HEAP_SIZE - BLOCK_HDR;
    free_list->used  = 0;
    free_list->next  = NULL;
    total_used       = 0;
}

void *os_malloc(uint32_t size) {
    if (size == 0)
        return NULL;

    /* 4-byte alignment */
    size = (size + 3) & ~3u;

    __disable_irq();

    block_t *cur = free_list;
    while (cur != NULL) {
        if (!cur->used && cur->size >= size) {
            /* Split when leftover exceeds header + 8 bytes */
            if (cur->size > size + BLOCK_HDR + 8) {
                block_t *fresh =
                    (block_t *)((uint8_t *)cur + BLOCK_HDR + size);
                fresh->size = cur->size - size - BLOCK_HDR;
                fresh->used = 0;
                fresh->next = cur->next;
                cur->size   = size;
                cur->next   = fresh;
            }
            cur->used   = 1;
            total_used += cur->size;
            __enable_irq();
            return (void *)((uint8_t *)cur + BLOCK_HDR);
        }
        cur = cur->next;
    }

    __enable_irq();
    return NULL;
}

void os_free(void *ptr) {
    if (ptr == NULL)
        return;

    __disable_irq();
    block_t *blk = (block_t *)((uint8_t *)ptr - BLOCK_HDR);
    blk->used     = 0;
    total_used   -= blk->size;

    /* Coalesce with the next block */
    if (blk->next != NULL && !blk->next->used) {
        blk->size += BLOCK_HDR + blk->next->size;
        blk->next  = blk->next->next;
    }
    __enable_irq();
}

uint32_t os_heap_used(void) {
    return total_used;
}

uint32_t os_heap_free(void) {
    return HEAP_SIZE - BLOCK_HDR - total_used;
}
