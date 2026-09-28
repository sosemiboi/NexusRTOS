/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

void    shell_task(void *arg);
uint8_t shell_get_crash_trigger(void);

#endif
