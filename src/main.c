/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "os_config.h"
#include "cortex_m3.h"
#include "scheduler.h"
#include "task.h"
#include "uart.h"
#include "shell.h"
#include "mutex.h"
#include "semaphore.h"
#include "msgqueue.h"
#include "heap.h"

/* ---- Shared resources for the demo --------------------------------- */
static mutex_t     print_mutex;
static msgqueue_t  demo_mq;
static uint8_t     mq_buf[MQ_MAX_MSGS * MQ_MSG_SIZE];

/* ---- Demo tasks ---------------------------------------------------- */

static void heartbeat_task(void *arg) {
    (void)arg;
    uint32_t count = 0;
    while (1) {
        mutex_lock(&print_mutex);
        uart_printf("[heartbeat] tick %u\r\n", count++);
        mutex_unlock(&print_mutex);
        os_task_delay(1000);
    }
}

static void producer_task(void *arg) {
    (void)arg;
    uint32_t seq = 0;
    while (1) {
        char msg[MQ_MSG_SIZE];
        int i = 0;
        const char *tag = "msg#";
        while (*tag) msg[i++] = *tag++;
        char num[12];
        int  n = 0;
        uint32_t v = seq;
        if (v == 0) num[n++] = '0';
        else while (v) { num[n++] = '0' + (v % 10); v /= 10; }
        while (n--) msg[i++] = num[n];
        msg[i] = '\0';

        mq_send(&demo_mq, msg);

        mutex_lock(&print_mutex);
        uart_printf("[producer ] sent: %s\r\n", msg);
        mutex_unlock(&print_mutex);

        seq++;
        os_task_delay(800);
    }
}

static void consumer_task(void *arg) {
    (void)arg;
    while (1) {
        char msg[MQ_MSG_SIZE];
        mq_recv(&demo_mq, msg);

        mutex_lock(&print_mutex);
        uart_printf("[consumer ] recv: %s\r\n", msg);
        mutex_unlock(&print_mutex);

        os_task_delay(1200);
    }
}

static void stats_task(void *arg) {
    (void)arg;
    while (1) {
        os_task_delay(5000);
        mutex_lock(&print_mutex);
        uart_printf("[stats    ] tasks=%u  heap=%u/%u  ticks=%u\r\n",
                    os_task_count(),
                    os_heap_used(), (unsigned)HEAP_SIZE,
                    os_get_ticks());
        mutex_unlock(&print_mutex);
    }
}

static void stress_task(void *arg) {
    (void)arg;
    while (1) {
        if (shell_get_crash_trigger()) {
            volatile char buf[2048];
            for (int i = 0; i < 2048; i++)
                buf[i] = (char)i;
            (void)buf[0];
        }
        os_task_delay(100);
    }
}

/* ---- Entry point --------------------------------------------------- */

int main(void) {
    uart_init();
    uart_puts("\r\n");
    uart_puts("  _   _                     ____  _____ ___  ____  \r\n");
    uart_puts(" | \\ | | _____  ___   _ ___|  _ \\|_   _/ _ \\/ ___| \r\n");
    uart_puts(" |  \\| |/ _ \\ \\/ / | | / __| |_) | | || | | \\___ \\ \r\n");
    uart_puts(" | |\\  |  __/>  <| |_| \\__ \\  _ <  | || |_| |___) |\r\n");
    uart_puts(" |_| \\_|\\___/_/\\_\\\\__,_|___/_| \\_\\ |_| \\___/|____/ \r\n");
    uart_puts("\r\n");
    uart_printf(" v%s | ARM Cortex-M3 | MIT License\r\n", NEXUS_VERSION_STR);
    uart_puts(" -----------------------------------------------\r\n\r\n");
    uart_puts(" Initializing kernel...\r\n");

    os_init();

    mutex_init(&print_mutex);
    mq_init(&demo_mq, mq_buf, MQ_MSG_SIZE, MQ_MAX_MSGS);

    os_task_create(heartbeat_task, NULL, 2, "heartbeat");
    os_task_create(producer_task,  NULL, 3, "producer");
    os_task_create(consumer_task,  NULL, 3, "consumer");
    os_task_create(stats_task,     NULL, 1, "stats");
    os_task_create(shell_task,     NULL, 4, "shell");

    int stress_id = os_task_create(stress_task, NULL, 1, "stress");
    if (stress_id >= 0)
        task_pool[stress_id].restartable = 1;

    uart_printf(" %u tasks created. Starting scheduler...\r\n",
                os_task_count());
    uart_puts(" [KTrace] Kernel event tracer active\r\n");
    uart_puts(" [Fault]  Stack canary monitor active\r\n\r\n");

    os_start();

    return 0;
}
