/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "mutex.h"
#include "scheduler.h"
#include "cortex_m3.h"
#include "ktrace.h"
#include <stddef.h>

void mutex_init(mutex_t *m) {
    m->owner      = NULL;
    m->lock       = 0;
    m->wait_queue = NULL;
}

void mutex_lock(mutex_t *m) {
    __disable_irq();

    if (m->lock == 0) {
        m->lock  = 1;
        m->owner = os_get_current_task();
        ktrace_log(EVT_MUTEX_LOCK, m->owner->id, 0);
        __enable_irq();
        return;
    }

    /* Already locked — priority inheritance */
    tcb_t *self = os_get_current_task();

    if (m->owner->priority < self->priority) {
        /* Boost owner to our priority so it runs sooner */
        os_ready_queue_remove(m->owner);
        m->owner->priority = self->priority;
        if (m->owner->state == TASK_READY)
            os_ready_queue_add(m->owner);
    }

    self->state = TASK_BLOCKED;
    self->next  = m->wait_queue;
    m->wait_queue = self;
    ktrace_log(EVT_MUTEX_BLOCK, self->id, m->owner->id);

    __enable_irq();
    trigger_pendsv();
}

void mutex_unlock(mutex_t *m) {
    __disable_irq();

    tcb_t *self = os_get_current_task();
    ktrace_log(EVT_MUTEX_UNLOCK, self->id, 0);

    /* Restore original priority after inheritance */
    if (self->priority != self->base_priority) {
        os_ready_queue_remove(self);
        self->priority = self->base_priority;
        if (self->state == TASK_READY)
            os_ready_queue_add(self);
    }

    if (m->wait_queue != NULL) {
        /* Transfer ownership to the first waiter */
        tcb_t *waiter  = m->wait_queue;
        m->wait_queue  = waiter->next;
        waiter->next   = NULL;
        waiter->state  = TASK_READY;
        m->owner       = waiter;
        os_ready_queue_add(waiter);
    } else {
        m->lock  = 0;
        m->owner = NULL;
    }

    __enable_irq();
    trigger_pendsv();
}
