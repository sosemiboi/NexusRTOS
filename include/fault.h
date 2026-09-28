/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef FAULT_H
#define FAULT_H

#include <stdint.h>
#include "task.h"

#define FAULT_LOG_SIZE 8

typedef struct {
    uint32_t tick;
    uint32_t pc;
    uint32_t lr;
    uint8_t  task_id;
    uint8_t  type;       /* 0 = stack overflow, 1 = hard fault */
} fault_record_t;

void     fault_init(void);
void     fault_check_canary(uint8_t task_idx);
void     fault_dump(void);
int      fault_restart_task(tcb_t *task);
uint32_t fault_total(void);

void HardFault_Handler_C(uint32_t *frame, uint32_t exc_lr);
void fault_recovery_stub(void);

#endif
