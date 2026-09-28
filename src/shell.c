/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "shell.h"
#include "uart.h"
#include "scheduler.h"
#include "heap.h"
#include "sysclock.h"
#include "task.h"
#include "os_config.h"
#include "ktrace.h"
#include "fault.h"

static volatile uint8_t crash_trigger;

static int str_eq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a++ != *b++) return 0;
    }
    return *a == *b;
}

static const char *state_name(task_state_t s) {
    switch (s) {
    case TASK_READY:      return "READY";
    case TASK_RUNNING:    return "RUN  ";
    case TASK_BLOCKED:    return "BLOCK";
    case TASK_SUSPENDED:  return "SUSP ";
    case TASK_TERMINATED: return "TERM ";
    }
    return "?????";
}

static uint32_t stack_usage(tcb_t *t) {
    uint32_t unused = 0;
    for (uint32_t i = STACK_CANARY_WORDS; i < STACK_SIZE; i++) {
        if (t->stack[i] == 0)
            unused++;
        else
            break;
    }
    return ((STACK_SIZE - STACK_CANARY_WORDS - unused) * 100) /
           (STACK_SIZE - STACK_CANARY_WORDS);
}

static void cmd_ps(void) {
    uart_printf("ID  NAME            PRI  STATE  STACK%%  FAULTS\r\n");
    uart_printf("--  --------------  ---  -----  ------  ------\r\n");
    for (int i = 0; i < MAX_TASKS; i++) {
        tcb_t *t = os_task_get((uint8_t)i);
        if (t->state == TASK_TERMINATED)
            continue;
        uart_printf("%d   %-14s  %d    %s  %u%%     %u\r\n",
                    t->id, t->name, t->priority,
                    state_name(t->state), stack_usage(t),
                    t->fault_count);
    }
}

static void cmd_top(void) {
    uint32_t uptime = os_get_ticks();
    if (uptime == 0) uptime = 1;

    uart_printf("ID  NAME            CPU%%   SWITCHES\r\n");
    uart_printf("--  --------------  -----  --------\r\n");
    for (int i = 0; i < MAX_TASKS; i++) {
        tcb_t *t = os_task_get((uint8_t)i);
        if (t->state == TASK_TERMINATED)
            continue;
        uint32_t pct = (t->run_ticks * 100) / uptime;
        uart_printf("%d   %-14s  %u%%     %u\r\n",
                    t->id, t->name, pct, t->switch_count);
    }
    uart_printf("(uptime: %u ticks)\r\n", uptime);
}

static void cmd_free(void) {
    uart_printf("Heap used : %u bytes\r\n", os_heap_used());
    uart_printf("Heap free : %u bytes\r\n", os_heap_free());
    uart_printf("Total     : %u bytes\r\n", (unsigned)HEAP_SIZE);
}

static void cmd_uptime(void) {
    uint32_t ticks = os_get_ticks();
    uint32_t sec   = ticks / 1000;
    uint32_t min   = sec / 60;
    sec %= 60;
    uart_printf("Uptime: %u min %u sec (%u ticks)\r\n", min, sec, ticks);
}

static void cmd_version(void) {
    uart_printf("NexusRTOS v%s\r\n", NEXUS_VERSION_STR);
    uart_puts("Target : ARM Cortex-M3 (LM3S6965)\r\n");
    uart_puts("License: MIT\r\n");
}

static void cmd_crash(void) {
    uart_puts("Triggering deliberate stack overflow on 'stress' task...\r\n");
    crash_trigger = 1;
}

static void cmd_help(void) {
    uart_puts("Commands:\r\n");
    uart_puts("  ps       - list active tasks\r\n");
    uart_puts("  top      - per-task CPU usage\r\n");
    uart_puts("  free     - heap memory usage\r\n");
    uart_puts("  uptime   - system uptime\r\n");
    uart_puts("  trace    - dump kernel event trace\r\n");
    uart_puts("  fault    - fault history log\r\n");
    uart_puts("  crash    - trigger test stack overflow\r\n");
    uart_puts("  version  - kernel version\r\n");
    uart_puts("  help     - this message\r\n");
}

static void shell_exec(const char *cmd) {
    if      (str_eq(cmd, "ps"))      cmd_ps();
    else if (str_eq(cmd, "top"))     cmd_top();
    else if (str_eq(cmd, "free"))    cmd_free();
    else if (str_eq(cmd, "uptime"))  cmd_uptime();
    else if (str_eq(cmd, "trace"))   ktrace_dump();
    else if (str_eq(cmd, "fault"))   fault_dump();
    else if (str_eq(cmd, "crash"))   cmd_crash();
    else if (str_eq(cmd, "version")) cmd_version();
    else if (str_eq(cmd, "help"))    cmd_help();
    else    uart_printf("nexus: unknown command '%s'\r\n", cmd);
}

uint8_t shell_get_crash_trigger(void) {
    uint8_t v = crash_trigger;
    crash_trigger = 0;
    return v;
}

void shell_task(void *arg) {
    (void)arg;
    char line[64];
    int  pos = 0;

    uart_puts("\r\n +-------------------------------+\r\n");
    uart_printf(" |  NexusRTOS Shell v%s         |\r\n", NEXUS_VERSION_STR);
    uart_puts(" |  Type 'help' for commands    |\r\n");
    uart_puts(" +-------------------------------+\r\n");
    uart_puts("nexus> ");

    while (1) {
        if (uart_available()) {
            char c = uart_getc();
            if (c == '\r' || c == '\n') {
                uart_puts("\r\n");
                line[pos] = '\0';
                if (pos > 0)
                    shell_exec(line);
                pos = 0;
                uart_puts("nexus> ");
            } else if (c == '\b' || c == 127) {
                if (pos > 0) {
                    pos--;
                    uart_puts("\b \b");
                }
            } else if (pos < (int)sizeof(line) - 1) {
                line[pos++] = c;
                uart_putc(c);
            }
        } else {
            os_task_yield();
        }
    }
}
