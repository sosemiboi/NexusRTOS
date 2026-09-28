/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "scheduler.h"
#include "sysclock.h"
#include "heap.h"
#include "cortex_m3.h"
#include "ktrace.h"
#include "fault.h"

/* ---- Kernel global state ------------------------------------------- */
tcb_t   task_pool[MAX_TASKS];
tcb_t  *current_task;

static tcb_t *ready_queue[MAX_PRIORITY_LEVELS];
static uint8_t task_count_val;

/* ---- Ready-queue helpers ------------------------------------------- */

void os_ready_queue_add(tcb_t *task) {
    uint8_t prio = task->priority;
    task->next = NULL;

    if (ready_queue[prio] == NULL) {
        ready_queue[prio] = task;
        return;
    }

    /* append to tail — gives FIFO / round-robin within the same prio */
    tcb_t *tail = ready_queue[prio];
    while (tail->next != NULL)
        tail = tail->next;
    tail->next = task;
}

void os_ready_queue_remove(tcb_t *task) {
    uint8_t prio = task->priority;
    tcb_t **pp = &ready_queue[prio];

    while (*pp != NULL) {
        if (*pp == task) {
            *pp = task->next;
            task->next = NULL;
            return;
        }
        pp = &(*pp)->next;
    }
}

/* ---- Scheduler ----------------------------------------------------- */

void os_schedule(void) {
    uint32_t now = os_get_ticks();

    /* Account CPU time for the outgoing task */
    if (current_task != NULL && current_task->state == TASK_RUNNING) {
        current_task->run_ticks += now - current_task->last_switched_in;
        current_task->state = TASK_READY;
        os_ready_queue_add(current_task);
    }

    tcb_t *prev = current_task;

    /* Pick the highest-priority ready task */
    for (int i = MAX_PRIORITY_LEVELS - 1; i >= 0; i--) {
        if (ready_queue[i] != NULL) {
            current_task = ready_queue[i];
            ready_queue[i] = current_task->next;
            current_task->next = NULL;
            current_task->state = TASK_RUNNING;
            current_task->last_switched_in = now;
            current_task->switch_count++;

            if (prev != current_task)
                ktrace_log(EVT_TASK_SWITCH, current_task->id,
                           prev ? prev->id : 0xFF);
            return;
        }
    }
}

/* ---- Init / Start -------------------------------------------------- */

static void idle_entry(void *arg) {
    (void)arg;
    while (1)
        __WFI();
}

void os_init(void) {
    for (int i = 0; i < MAX_PRIORITY_LEVELS; i++)
        ready_queue[i] = NULL;

    for (int i = 0; i < MAX_TASKS; i++)
        task_pool[i].state = TASK_TERMINATED;

    current_task   = NULL;
    task_count_val = 0;

    os_heap_init();
    ktrace_init();
    fault_init();

    /* The idle task lives at priority 0 -- always schedulable */
    os_task_create(idle_entry, NULL, 0, "idle");
}

void os_start(void) {
    __disable_irq();

    /* Set PendSV and SysTick to the lowest exception priority */
    SCB_SHPR3 |= (0xFFU << 16);   /* PendSV  prio = 0xFF */
    SCB_SHPR3 |= (0xFFU << 24);   /* SysTick prio = 0xFF */

    systick_init();

    /* Signal "no previous task" so PendSV skips the first save */
    __set_PSP(0);

    /* Kick the first context switch */
    trigger_pendsv();
    __enable_irq();

    /* We never return — PendSV takes over */
    while (1)
        ;
}

/* ---- Accessors ----------------------------------------------------- */

tcb_t *os_get_current_task(void) {
    return current_task;
}

uint8_t os_task_count(void) {
    return task_count_val;
}

void os_task_count_inc(void) {
    task_count_val++;
}

void os_task_count_dec(void) {
    if (task_count_val > 0)
        task_count_val--;
}
