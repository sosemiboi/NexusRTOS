/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "ktrace.h"
#include "sysclock.h"
#include "uart.h"

static trace_event_t trace_buf[TRACE_BUF_SIZE];
static volatile uint32_t trace_head;
static volatile uint32_t trace_count;

void ktrace_init(void) {
    trace_head  = 0;
    trace_count = 0;
}

void ktrace_log(uint8_t type, uint8_t task_id, uint16_t extra) {
    trace_event_t *evt = &trace_buf[trace_head];
    evt->timestamp = os_get_ticks();
    evt->type      = type;
    evt->task_id   = task_id;
    evt->extra     = extra;
    trace_head = (trace_head + 1) % TRACE_BUF_SIZE;
    if (trace_count < TRACE_BUF_SIZE)
        trace_count++;
}

static const char *evt_name(uint8_t t) {
    switch (t) {
    case EVT_TASK_SWITCH:  return "SWITCH  ";
    case EVT_TASK_CREATE:  return "CREATE  ";
    case EVT_TASK_DELETE:  return "DELETE  ";
    case EVT_TASK_DELAY:   return "DELAY   ";
    case EVT_TASK_WAKE:    return "WAKE    ";
    case EVT_MUTEX_LOCK:   return "MTX_LOCK";
    case EVT_MUTEX_UNLOCK: return "MTX_UNLK";
    case EVT_MUTEX_BLOCK:  return "MTX_BLCK";
    case EVT_FAULT:        return "FAULT   ";
    case EVT_STACK_OVF:    return "STK_OVF ";
    case EVT_TASK_RESTART: return "RESTART ";
    }
    return "????????";
}

void ktrace_dump(void) {
    if (trace_count == 0) {
        uart_puts("(trace buffer empty)\r\n");
        return;
    }

    uart_puts("TICK       EVENT      TID  EXTRA\r\n");
    uart_puts("---------  --------   ---  -----\r\n");

    uint32_t start;
    uint32_t n = trace_count;
    if (n >= TRACE_BUF_SIZE)
        start = trace_head;
    else
        start = (trace_head + TRACE_BUF_SIZE - n) % TRACE_BUF_SIZE;

    /* Show the last 32 events to keep output manageable */
    if (n > 32) {
        start = (trace_head + TRACE_BUF_SIZE - 32) % TRACE_BUF_SIZE;
        n = 32;
    }

    for (uint32_t i = 0; i < n; i++) {
        uint32_t idx = (start + i) % TRACE_BUF_SIZE;
        trace_event_t *e = &trace_buf[idx];
        uart_printf("%-9u  %s   %u    %u\r\n",
                    e->timestamp, evt_name(e->type),
                    e->task_id, e->extra);
    }
    uart_printf("(%u events total, showing last %u)\r\n", trace_count, n);
}

void ktrace_clear(void) {
    trace_head  = 0;
    trace_count = 0;
    uart_puts("Trace buffer cleared.\r\n");
}
