/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

extern tcb_t  task_pool[];
extern tcb_t *current_task;      /* also referenced from context_switch.s */

void    os_init(void);
void    os_start(void);
void    os_schedule(void);       /* called from PendSV -- picks next task */
void    os_ready_queue_add(tcb_t *task);
void    os_ready_queue_remove(tcb_t *task);
tcb_t  *os_get_current_task(void);
uint32_t os_get_tick_count(void);
void    os_task_count_dec(void);

#endif
