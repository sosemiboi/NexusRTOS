/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "msgqueue.h"
#include "scheduler.h"
#include "cortex_m3.h"
#include <stddef.h>

static void mem_copy(void *dst, const void *src, uint32_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n--)
        *d++ = *s++;
}

void mq_init(msgqueue_t *mq, void *buffer, uint32_t msg_size,
             uint32_t capacity) {
    mq->buffer    = (uint8_t *)buffer;
    mq->msg_size  = msg_size;
    mq->capacity  = capacity;
    mq->count     = 0;
    mq->head      = 0;
    mq->tail      = 0;
    mq->send_wait = NULL;
    mq->recv_wait = NULL;
}

int mq_send(msgqueue_t *mq, const void *msg) {
    __disable_irq();

    if (mq->count >= mq->capacity) {
        /* Queue full — block sender */
        tcb_t *self   = os_get_current_task();
        self->state   = TASK_BLOCKED;
        self->next    = mq->send_wait;
        mq->send_wait = self;
        __enable_irq();
        trigger_pendsv();

        /* We resume here after being unblocked — retry */
        __disable_irq();
    }

    /* Copy message into the ring buffer */
    mem_copy(mq->buffer + mq->tail * mq->msg_size, msg, mq->msg_size);
    mq->tail = (mq->tail + 1) % mq->capacity;
    mq->count++;

    /* Wake a blocked receiver if any */
    if (mq->recv_wait != NULL) {
        tcb_t *waiter  = mq->recv_wait;
        mq->recv_wait  = waiter->next;
        waiter->next   = NULL;
        waiter->state  = TASK_READY;
        os_ready_queue_add(waiter);
    }

    __enable_irq();
    trigger_pendsv();
    return 0;
}

int mq_recv(msgqueue_t *mq, void *msg) {
    __disable_irq();

    if (mq->count == 0) {
        /* Queue empty — block receiver */
        tcb_t *self   = os_get_current_task();
        self->state   = TASK_BLOCKED;
        self->next    = mq->recv_wait;
        mq->recv_wait = self;
        __enable_irq();
        trigger_pendsv();

        __disable_irq();
    }

    /* Copy message out of the ring buffer */
    mem_copy(msg, mq->buffer + mq->head * mq->msg_size, mq->msg_size);
    mq->head = (mq->head + 1) % mq->capacity;
    mq->count--;

    /* Wake a blocked sender if any */
    if (mq->send_wait != NULL) {
        tcb_t *waiter  = mq->send_wait;
        mq->send_wait  = waiter->next;
        waiter->next   = NULL;
        waiter->state  = TASK_READY;
        os_ready_queue_add(waiter);
    }

    __enable_irq();
    trigger_pendsv();
    return 0;
}
