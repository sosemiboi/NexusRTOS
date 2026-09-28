/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "sysclock.h"
#include "scheduler.h"
#include "cortex_m3.h"
#include "os_config.h"
#include "fault.h"

static volatile uint32_t tick_count;

void systick_init(void) {
    /* Reload value for 1 ms ticks: (CPU_FREQ / 1000) - 1 */
    SYST_RVR = (CPU_FREQ_HZ / SYSTICK_FREQ_HZ) - 1;
    SYST_CVR = 0;
    SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
}

uint32_t os_get_ticks(void) {
    return tick_count;
}

/* ---- SysTick ISR --------------------------------------------------- */

static uint8_t canary_scan_idx;

void SysTick_Handler(void) {
    tick_count++;

    /* Walk every task slot -- wake those whose delay has expired */
    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_pool[i].state == TASK_BLOCKED &&
            task_pool[i].delay_ticks > 0) {
            task_pool[i].delay_ticks--;
            if (task_pool[i].delay_ticks == 0) {
                task_pool[i].state = TASK_READY;
                os_ready_queue_add(&task_pool[i]);
            }
        }
    }

    /* Round-robin stack canary check -- one task per tick */
    fault_check_canary(canary_scan_idx);
    canary_scan_idx = (canary_scan_idx + 1) % MAX_TASKS;

    /* Request a context switch (PendSV is lowest priority) */
    trigger_pendsv();
}
