/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef MUTEX_H
#define MUTEX_H

#include "task.h"

typedef struct {
    tcb_t   *owner;
    uint8_t  lock;
    tcb_t   *wait_queue;
} mutex_t;

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);

#endif
