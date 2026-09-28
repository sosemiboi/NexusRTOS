/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

void     os_heap_init(void);
void    *os_malloc(uint32_t size);
void     os_free(void *ptr);
uint32_t os_heap_used(void);
uint32_t os_heap_free(void);

#endif
