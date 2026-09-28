/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#define NEXUS_VERSION_MAJOR 1
#define NEXUS_VERSION_MINOR 0
#define NEXUS_VERSION_PATCH 0
#define NEXUS_VERSION_STR   "1.0.0"

#define MAX_TASKS           16
#define MAX_PRIORITY_LEVELS 8
#define STACK_SIZE          512     /* words (2048 bytes per task) */
#define HEAP_SIZE           (16 * 1024)
#define SYSTICK_FREQ_HZ     1000
#define CPU_FREQ_HZ         50000000  /* 50 MHz -- LM3S6965 on QEMU */

#define MQ_MAX_MSGS         16
#define MQ_MSG_SIZE         32

#define TRACE_BUF_SIZE      256
#define STACK_CANARY        0xDEADBEEFu
#define STACK_CANARY_WORDS  4

#endif
