/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef SYSCLOCK_H
#define SYSCLOCK_H

#include <stdint.h>

void     systick_init(void);
uint32_t os_get_ticks(void);

#endif
