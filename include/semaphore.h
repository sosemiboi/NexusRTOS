/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include "task.h"

typedef struct {
    volatile int32_t count;
    tcb_t *wait_queue;
} semaphore_t;

void sem_init(semaphore_t *s, int32_t initial_count);
void sem_wait(semaphore_t *s);
void sem_post(semaphore_t *s);

#endif
