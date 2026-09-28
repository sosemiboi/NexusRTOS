/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <stdint.h>
#include "task.h"

typedef struct {
    uint8_t  *buffer;
    uint32_t  msg_size;
    uint32_t  capacity;
    volatile uint32_t count;
    uint32_t  head;
    uint32_t  tail;
    tcb_t    *send_wait;
    tcb_t    *recv_wait;
} msgqueue_t;

void mq_init(msgqueue_t *mq, void *buffer, uint32_t msg_size, uint32_t capacity);
int  mq_send(msgqueue_t *mq, const void *msg);
int  mq_recv(msgqueue_t *mq, void *msg);

#endif
