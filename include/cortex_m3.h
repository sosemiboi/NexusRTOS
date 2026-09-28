/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#ifndef CORTEX_M3_H
#define CORTEX_M3_H

#include <stdint.h>

/* ---- SysTick ---- */
#define SYST_CSR   (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR   (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR   (*(volatile uint32_t *)0xE000E018)

#define SYST_CSR_ENABLE     (1 << 0)
#define SYST_CSR_TICKINT    (1 << 1)
#define SYST_CSR_CLKSOURCE  (1 << 2)

/* ---- SCB ---- */
#define SCB_ICSR   (*(volatile uint32_t *)0xE000ED04)
#define SCB_SHPR3  (*(volatile uint32_t *)0xE000ED20)

#define SCB_ICSR_PENDSVSET  (1 << 28)
#define SCB_ICSR_PENDSVCLR  (1 << 27)

/* ---- NVIC ---- */
#define NVIC_ISER0 (*(volatile uint32_t *)0xE000E100)

/* ---- UART0 (LM3S6965) ---- */
#define UART0_BASE   0x4000C000
#define UART0_DR     (*(volatile uint32_t *)(UART0_BASE + 0x000))
#define UART0_FR     (*(volatile uint32_t *)(UART0_BASE + 0x018))
#define UART0_IBRD   (*(volatile uint32_t *)(UART0_BASE + 0x024))
#define UART0_FBRD   (*(volatile uint32_t *)(UART0_BASE + 0x028))
#define UART0_LCRH   (*(volatile uint32_t *)(UART0_BASE + 0x02C))
#define UART0_CTL    (*(volatile uint32_t *)(UART0_BASE + 0x030))

#define UART_FR_RXFE (1 << 4)  /* RX FIFO empty */
#define UART_FR_TXFF (1 << 5)  /* TX FIFO full  */

/* ---- System Control ---- */
#define SYSCTL_RCGC1 (*(volatile uint32_t *)0x400FE104)
#define SYSCTL_RCGC2 (*(volatile uint32_t *)0x400FE108)

/* ---- Intrinsics ---- */

static inline void __disable_irq(void) {
    __asm volatile ("cpsid i" ::: "memory");
}

static inline void __enable_irq(void) {
    __asm volatile ("cpsie i" ::: "memory");
}

static inline void __set_PSP(uint32_t val) {
    __asm volatile ("MSR psp, %0" :: "r"(val));
}

static inline uint32_t __get_PSP(void) {
    uint32_t val;
    __asm volatile ("MRS %0, psp" : "=r"(val));
    return val;
}

static inline void __ISB(void) {
    __asm volatile ("isb" ::: "memory");
}

static inline void __DSB(void) {
    __asm volatile ("dsb" ::: "memory");
}

static inline void __WFI(void) {
    __asm volatile ("wfi");
}

static inline void trigger_pendsv(void) {
    SCB_ICSR |= SCB_ICSR_PENDSVSET;
}

#endif
