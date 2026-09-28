/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "os_config.h"

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED,
    TASK_TERMINATED
} task_state_t;

typedef void (*task_func_t)(void *arg);

typedef struct tcb {
    uint32_t    *stack_ptr;       /* MUST be first -- used by context_switch.s */
    uint32_t     stack[STACK_SIZE];
    char         name[16];
    uint8_t      priority;
    uint8_t      base_priority;
    task_state_t state;
    uint32_t     delay_ticks;
    uint8_t      id;
    struct tcb  *next;            /* linked-list pointer for ready / wait queues */
    task_func_t  entry_func;
    void        *entry_arg;
    uint8_t      restartable;
    uint32_t     run_ticks;
    uint32_t     last_switched_in;
    uint32_t     switch_count;
    uint32_t     fault_count;
} tcb_t;

int      os_task_create(task_func_t func, void *arg, uint8_t priority,
                        const char *name);
void     os_task_delay(uint32_t ticks);
void     os_task_yield(void);
void     os_task_delete(uint8_t task_id);
tcb_t   *os_task_get(uint8_t id);
uint8_t  os_task_count(void);

#endif
