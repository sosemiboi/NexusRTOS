/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "semaphore.h"
#include "scheduler.h"
#include "cortex_m3.h"
#include <stddef.h>

void sem_init(semaphore_t *s, int32_t initial_count) {
    s->count      = initial_count;
    s->wait_queue = NULL;
}

void sem_wait(semaphore_t *s) {
    __disable_irq();

    if (s->count > 0) {
        s->count--;
        __enable_irq();
        return;
    }

    /* Block the caller */
    tcb_t *self = os_get_current_task();
    self->state = TASK_BLOCKED;
    self->next  = s->wait_queue;
    s->wait_queue = self;

    __enable_irq();
    trigger_pendsv();
}

void sem_post(semaphore_t *s) {
    __disable_irq();

    if (s->wait_queue != NULL) {
        /* Wake one waiter directly */
        tcb_t *waiter  = s->wait_queue;
        s->wait_queue  = waiter->next;
        waiter->next   = NULL;
        waiter->state  = TASK_READY;
        os_ready_queue_add(waiter);
    } else {
        s->count++;
    }

    __enable_irq();
    trigger_pendsv();
}
