/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef KTRACE_H
#define KTRACE_H

#include <stdint.h>
#include "os_config.h"

typedef enum {
    EVT_TASK_SWITCH  = 0,
    EVT_TASK_CREATE  = 1,
    EVT_TASK_DELETE  = 2,
    EVT_TASK_DELAY   = 3,
    EVT_TASK_WAKE    = 4,
    EVT_MUTEX_LOCK   = 5,
    EVT_MUTEX_UNLOCK = 6,
    EVT_MUTEX_BLOCK  = 7,
    EVT_FAULT        = 8,
    EVT_STACK_OVF    = 9,
    EVT_TASK_RESTART = 10,
} trace_evt_t;

typedef struct {
    uint32_t timestamp;
    uint8_t  type;
    uint8_t  task_id;
    uint16_t extra;
} trace_event_t;

void ktrace_init(void);
void ktrace_log(uint8_t type, uint8_t task_id, uint16_t extra);
void ktrace_dump(void);
void ktrace_clear(void);

#endif
