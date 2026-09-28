/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "fault.h"
#include "scheduler.h"
#include "sysclock.h"
#include "ktrace.h"
#include "uart.h"
#include "cortex_m3.h"
#include "os_config.h"

static fault_record_t fault_log[FAULT_LOG_SIZE];
static uint32_t fault_head;
static uint32_t fault_count;

void fault_init(void) {
    fault_head  = 0;
    fault_count = 0;
}

static void record_fault(uint8_t task_id, uint8_t type, uint32_t pc,
                          uint32_t lr) {
    fault_record_t *r = &fault_log[fault_head];
    r->tick    = os_get_ticks();
    r->pc      = pc;
    r->lr      = lr;
    r->task_id = task_id;
    r->type    = type;
    fault_head = (fault_head + 1) % FAULT_LOG_SIZE;
    if (fault_count < FAULT_LOG_SIZE)
        fault_count++;
}

int fault_restart_task(tcb_t *task) {
    if (task->entry_func == 0)
        return -1;

    ktrace_log(EVT_TASK_RESTART, task->id, task->fault_count);

    task->state       = TASK_READY;
    task->delay_ticks = 0;
    task->next        = NULL;
    task->priority    = task->base_priority;
    task->fault_count++;

    /* Re-paint stack canaries */
    for (int i = 0; i < STACK_CANARY_WORDS; i++)
        task->stack[i] = STACK_CANARY;

    /* Re-initialize the stack frame pointing to the original entry */
    uint32_t *sp = &task->stack[STACK_SIZE];
    *(--sp) = 0x01000000u;
    *(--sp) = (uint32_t)task->entry_func;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = 0;
    *(--sp) = (uint32_t)task->entry_arg;
    *(--sp) = 0; *(--sp) = 0; *(--sp) = 0; *(--sp) = 0;
    *(--sp) = 0; *(--sp) = 0; *(--sp) = 0; *(--sp) = 0;
    task->stack_ptr = sp;

    os_ready_queue_add(task);
    return 0;
}

void fault_check_canary(uint8_t task_idx) {
    tcb_t *t = &task_pool[task_idx];
    if (t->state == TASK_TERMINATED)
        return;

    for (int i = 0; i < STACK_CANARY_WORDS; i++) {
        if (t->stack[i] != STACK_CANARY) {
            ktrace_log(EVT_STACK_OVF, t->id, 0);
            record_fault(t->id, 0, 0, 0);

            uart_printf("\r\n[FAULT] Stack overflow: task '%s' (id=%u)\r\n",
                        t->name, t->id);

            t->state = TASK_TERMINATED;
            os_ready_queue_remove(t);

            if (t->restartable && fault_restart_task(t) == 0) {
                uart_printf("[FAULT] Task '%s' auto-restarted "
                            "(crash #%u)\r\n", t->name, t->fault_count);
            } else {
                uart_printf("[FAULT] Task '%s' terminated.\r\n", t->name);
                extern void os_task_count_dec(void);
                os_task_count_dec();
            }

            if (t == current_task)
                trigger_pendsv();

            return;
        }
    }
}

void HardFault_Handler_C(uint32_t *frame, uint32_t exc_lr) {
    (void)exc_lr;
    uint32_t stacked_pc = frame[6];
    uint32_t stacked_lr = frame[5];

    uint8_t tid = current_task ? current_task->id : 0xFF;

    ktrace_log(EVT_FAULT, tid, (uint16_t)(stacked_pc & 0xFFFF));
    record_fault(tid, 1, stacked_pc, stacked_lr);

    uart_printf("\r\n[HARDFAULT] task='%s' id=%u PC=0x%x LR=0x%x\r\n",
                current_task ? current_task->name : "??",
                tid, stacked_pc, stacked_lr);

    if (current_task) {
        current_task->state = TASK_TERMINATED;

        if (current_task->restartable &&
            fault_restart_task(current_task) == 0) {
            uart_printf("[HARDFAULT] Task auto-restarted.\r\n");
        } else {
            uart_printf("[HARDFAULT] Task terminated.\r\n");
            extern void os_task_count_dec(void);
            os_task_count_dec();
        }

        /* Redirect return: set stacked PC to a recovery stub */
        frame[6] = (uint32_t)fault_recovery_stub;
    }
}

/* Runs in thread mode after HardFault return */
void __attribute__((naked)) fault_recovery_stub(void) {
    __asm volatile(
        "CPSIE I          \n"
        "BL trigger_pendsv\n"
        "1: B 1b          \n"
    );
}

void fault_dump(void) {
    if (fault_count == 0) {
        uart_puts("No faults recorded.\r\n");
        return;
    }

    uart_puts("TICK       TYPE       TID  PC          LR\r\n");
    uart_puts("---------  ---------  ---  ----------  ----------\r\n");

    uint32_t start;
    uint32_t n = fault_count;
    if (n >= FAULT_LOG_SIZE)
        start = fault_head;
    else
        start = 0;

    for (uint32_t i = 0; i < n; i++) {
        uint32_t idx = (start + i) % FAULT_LOG_SIZE;
        fault_record_t *r = &fault_log[idx];
        uart_printf("%-9u  %-9s  %u    0x%x  0x%x\r\n",
                    r->tick,
                    r->type == 0 ? "STK_OVF" : "HARDFLT",
                    r->task_id, r->pc, r->lr);
    }
    uart_printf("(%u faults total)\r\n", fault_count);
}

uint32_t fault_total(void) {
    return fault_count;
}
