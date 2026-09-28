/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "task.h"
#include "scheduler.h"
#include "cortex_m3.h"
#include "ktrace.h"
#include <stddef.h>

/* forward */
extern void os_task_count_inc(void);
extern void os_task_count_dec(void);

static void task_return_handler(void) {
    tcb_t *self = os_get_current_task();
    if (self)
        os_task_delete(self->id);
    while (1)
        ;
}

static void task_init_stack(tcb_t *tcb, task_func_t func, void *arg) {
    uint32_t *sp = &tcb->stack[STACK_SIZE];

    /* Hardware-saved exception frame (high addr → low addr) */
    *(--sp) = 0x01000000u;                  /* xPSR  — Thumb bit        */
    *(--sp) = (uint32_t)func;               /* PC                       */
    *(--sp) = (uint32_t)task_return_handler; /* LR                       */
    *(--sp) = 0;                            /* R12                      */
    *(--sp) = 0;                            /* R3                       */
    *(--sp) = 0;                            /* R2                       */
    *(--sp) = 0;                            /* R1                       */
    *(--sp) = (uint32_t)arg;                /* R0                       */

    /* Manually-saved registers */
    *(--sp) = 0;  /* R11 */
    *(--sp) = 0;  /* R10 */
    *(--sp) = 0;  /* R9  */
    *(--sp) = 0;  /* R8  */
    *(--sp) = 0;  /* R7  */
    *(--sp) = 0;  /* R6  */
    *(--sp) = 0;  /* R5  */
    *(--sp) = 0;  /* R4  */

    tcb->stack_ptr = sp;
}

static int str_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (i < max - 1 && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return i;
}

/* ---- Public API ---------------------------------------------------- */

int os_task_create(task_func_t func, void *arg, uint8_t priority,
                   const char *name) {
    if (priority >= MAX_PRIORITY_LEVELS)
        return -1;

    __disable_irq();

    /* Find a free slot */
    int slot = -1;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_pool[i].state == TASK_TERMINATED) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        __enable_irq();
        return -1;
    }

    tcb_t *tcb = &task_pool[slot];
    tcb->id             = (uint8_t)slot;
    tcb->priority       = priority;
    tcb->base_priority  = priority;
    tcb->state          = TASK_READY;
    tcb->delay_ticks    = 0;
    tcb->next           = NULL;
    tcb->entry_func     = func;
    tcb->entry_arg      = arg;
    tcb->restartable    = 0;
    tcb->run_ticks      = 0;
    tcb->last_switched_in = 0;
    tcb->switch_count   = 0;
    tcb->fault_count    = 0;

    /* Paint stack canaries at the bottom of the stack */
    for (int i = 0; i < STACK_CANARY_WORDS; i++)
        tcb->stack[i] = STACK_CANARY;

    str_copy(tcb->name, name ? name : "?", sizeof(tcb->name));
    task_init_stack(tcb, func, arg);

    os_ready_queue_add(tcb);
    os_task_count_inc();
    ktrace_log(EVT_TASK_CREATE, (uint8_t)slot, priority);

    __enable_irq();
    return slot;
}

void os_task_delay(uint32_t ticks) {
    __disable_irq();
    tcb_t *self      = os_get_current_task();
    self->delay_ticks = ticks;
    self->state       = TASK_BLOCKED;
    ktrace_log(EVT_TASK_DELAY, self->id, (uint16_t)(ticks & 0xFFFF));
    __enable_irq();

    trigger_pendsv();
}

void os_task_yield(void) {
    trigger_pendsv();
}

void os_task_delete(uint8_t task_id) {
    if (task_id >= MAX_TASKS)
        return;

    __disable_irq();
    tcb_t *tcb = &task_pool[task_id];
    tcb->state = TASK_TERMINATED;
    os_ready_queue_remove(tcb);
    os_task_count_dec();
    ktrace_log(EVT_TASK_DELETE, task_id, 0);
    __enable_irq();

    if (tcb == os_get_current_task())
        trigger_pendsv();
}

tcb_t *os_task_get(uint8_t id) {
    if (id >= MAX_TASKS)
        return NULL;
    return &task_pool[id];
}
