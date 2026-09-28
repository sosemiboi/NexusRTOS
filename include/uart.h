/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_putc(char c);
char uart_getc(void);
int  uart_available(void);
void uart_puts(const char *s);
void uart_printf(const char *fmt, ...);

#endif
