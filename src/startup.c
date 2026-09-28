/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

#include <stdint.h>

/* Symbols from linker script */
extern uint32_t _estack;
extern uint32_t _sidata, _sdata, _edata;
extern uint32_t _sbss, _ebss;

extern int main(void);

void Reset_Handler(void);
void Default_Handler(void);
void HardFault_Handler(void) __attribute__((weak));

/* Weak aliases — override in kernel code */
void NMI_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler (void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler       (void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler  (void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler    (void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler   (void) __attribute__((weak, alias("Default_Handler")));

/* ---- Vector table -------------------------------------------------- */
__attribute__((section(".isr_vector"), used))
void (* const vector_table[])(void) = {
    (void (*)(void))((uint32_t)&_estack),   /*  0  Initial SP            */
    Reset_Handler,                          /*  1  Reset                 */
    NMI_Handler,                            /*  2  NMI                   */
    HardFault_Handler,                      /*  3  Hard fault            */
    MemManage_Handler,                      /*  4  Memory management     */
    BusFault_Handler,                       /*  5  Bus fault             */
    UsageFault_Handler,                     /*  6  Usage fault           */
    0, 0, 0, 0,                             /*  7-10  Reserved           */
    SVC_Handler,                            /* 11  SVCall                */
    DebugMon_Handler,                       /* 12  Debug monitor         */
    0,                                      /* 13  Reserved              */
    PendSV_Handler,                         /* 14  PendSV                */
    SysTick_Handler,                        /* 15  SysTick               */
};

/* ---- Reset handler ------------------------------------------------- */
void Reset_Handler(void) {
    /* Copy .data section from flash to SRAM */
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
        *dst++ = *src++;

    /* Zero .bss */
    dst = &_sbss;
    while (dst < &_ebss)
        *dst++ = 0;

    main();

    while (1)
        ;
}

void Default_Handler(void) {
    while (1)
        ;
}

void __attribute__((weak)) HardFault_Handler(void) {
    while (1)
        ;
}
