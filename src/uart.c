/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include "uart.h"
#include "cortex_m3.h"
#include <stdarg.h>

void uart_init(void) {
    /* Enable UART0 and GPIOA clocks */
    SYSCTL_RCGC1 |= (1 << 0);   /* UART0 */
    SYSCTL_RCGC2 |= (1 << 0);   /* GPIOA */

    /* Brief delay for clock to settle */
    volatile int d = 0;
    (void)d;

    /* Disable UART while configuring */
    UART0_CTL &= ~(1 << 0);

    /* 115200 baud @ 50 MHz:  divider = 50e6 / (16 * 115200) = 27.1267 */
    UART0_IBRD = 27;
    UART0_FBRD = 8;    /* frac = round(0.1267 * 64) = 8 */

    /* 8-N-1, enable FIFOs */
    UART0_LCRH = (0x3 << 5) | (1 << 4);

    /* Enable UART, TX, RX */
    UART0_CTL = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
    while (UART0_FR & UART_FR_TXFF)
        ;
    UART0_DR = (uint32_t)c;
}

char uart_getc(void) {
    while (UART0_FR & UART_FR_RXFE)
        ;
    return (char)(UART0_DR & 0xFF);
}

int uart_available(void) {
    return !(UART0_FR & UART_FR_RXFE);
}

void uart_puts(const char *s) {
    while (*s)
        uart_putc(*s++);
}

/* ---- Minimal printf ------------------------------------------------ */

static void print_uint(uint32_t v) {
    char buf[12];
    int  i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v) { buf[i++] = '0' + (v % 10); v /= 10; }
    while (i--) uart_putc(buf[i]);
}

static void print_int(int32_t v) {
    if (v < 0) { uart_putc('-'); v = -v; }
    print_uint((uint32_t)v);
}

static void print_hex(uint32_t v) {
    static const char hex[] = "0123456789abcdef";
    uart_putc('0'); uart_putc('x');
    for (int i = 28; i >= 0; i -= 4)
        uart_putc(hex[(v >> i) & 0xF]);
}

void uart_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    while (*fmt) {
        if (*fmt != '%') {
            uart_putc(*fmt++);
            continue;
        }
        fmt++;
        switch (*fmt) {
        case 'd': print_int(va_arg(ap, int));          break;
        case 'u': print_uint(va_arg(ap, unsigned));    break;
        case 'x': print_hex(va_arg(ap, unsigned));     break;
        case 's': uart_puts(va_arg(ap, const char *)); break;
        case 'c': uart_putc((char)va_arg(ap, int));    break;
        case '%': uart_putc('%');                       break;
        default:  uart_putc('%'); uart_putc(*fmt);     break;
        }
        fmt++;
    }

    va_end(ap);
}
