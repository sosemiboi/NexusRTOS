/*
 * NexusRTOS - Real-Time Operating System for ARM Cortex-M
 * Copyright (c) 2026 - MIT License (see LICENSE)
 */

/*
 * context_switch.s — PendSV handler for Cortex-M3
 *
 * On exception entry the hardware pushes {R0-R3, R12, LR, PC, xPSR}
 * onto the *process* stack (PSP).  We manually save/restore R4-R11.
 *
 * TCB layout: the very first field is `uint32_t *stack_ptr`, so
 * loading/storing at offset 0 of the TCB pointer gives us the SP.
 */

    .syntax unified
    .thumb
    .cpu    cortex-m3

    .extern current_task
    .extern os_schedule

/* -------------------------------------------------------------------- */
    .global PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    CPSID   I                       /* disable interrupts              */

    MRS     R0, PSP                 /* R0 = current process SP         */
    CBZ     R0, .Lskip_save         /* first switch — nothing to save  */

    /* Save R4-R11 onto the current task's stack */
    STMDB   R0!, {R4-R11}

    /* current_task->stack_ptr = R0 */
    LDR     R1, =current_task
    LDR     R1, [R1]                /* R1 = &current_tcb               */
    STR     R0, [R1, #0]            /* tcb.stack_ptr = R0              */

.Lskip_save:
    /* Call the C scheduler to pick the next task */
    PUSH    {LR}
    BL      os_schedule
    POP     {LR}

    /* Load next task's stack pointer */
    LDR     R1, =current_task
    LDR     R1, [R1]                /* R1 = &next_tcb                  */
    LDR     R0, [R1, #0]            /* R0 = next_tcb.stack_ptr         */

    /* Restore R4-R11 from the next task's stack */
    LDMIA   R0!, {R4-R11}

    /* Set PSP to the (now updated) stack pointer */
    MSR     PSP, R0

    /* Ensure EXC_RETURN uses PSP in thread mode (bit 2 = 1) */
    ORR     LR, LR, #0x04

    CPSIE   I                       /* re-enable interrupts            */
    BX      LR                      /* exception return → next task    */

    .size PendSV_Handler, . - PendSV_Handler

/* -------------------------------------------------------------------- */
    .extern HardFault_Handler_C

    .global HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    TST     LR, #4
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP
    MOV     R1, LR
    B       HardFault_Handler_C

    .size HardFault_Handler, . - HardFault_Handler
