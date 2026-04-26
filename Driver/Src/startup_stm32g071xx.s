/**
 * startup_stm32g071xx.s
 *
 * STM32G071 Cortex-M0+ reset handler and vector table.
 * Compatible with arm-none-eabi-gcc / GNU Assembler.
 */

    .syntax unified
    .cpu cortex-m0plus
    .thumb

/* =========================================================== */
/*  Stack and heap sizes — override via -D<SYM>=<val>           */
/* =========================================================== */
    .equ    Stack_Size, 0x00000400    /* 1 KB */
    .equ    Heap_Size,  0x00000200    /* 512 B */

/* =========================================================== */
/*  Stack / Heap reservations in .bss-like sections             */
/* =========================================================== */
    .section .stack, "aw", %nobits
    .align 3
    .space Stack_Size
__StackTop:

    .section .heap, "aw", %nobits
    .align 3
__HeapBase:
    .space Heap_Size
__HeapLimit:

/* =========================================================== */
/*  Vector Table                                                 */
/* =========================================================== */
    .section .isr_vector, "a", %progbits
    .type g_pfnVectors, %object
    .size g_pfnVectors, .-g_pfnVectors
g_pfnVectors:
    /* Cortex-M0+ exceptions */
    .word   __StackTop                      /* 0: Initial Stack Pointer   */
    .word   Reset_Handler                   /* 1: Reset                   */
    .word   NMI_Handler                     /* 2: NMI                     */
    .word   HardFault_Handler               /* 3: Hard Fault              */
    .word   0                               /* 4: Reserved                */
    .word   0                               /* 5: Reserved                */
    .word   0                               /* 6: Reserved                */
    .word   0                               /* 7: Reserved                */
    .word   0                               /* 8: Reserved                */
    .word   0                               /* 9: Reserved                */
    .word   0                               /*10: Reserved                */
    .word   SVC_Handler                     /*11: SVCall                  */
    .word   0                               /*12: Reserved                */
    .word   0                               /*13: Reserved                */
    .word   PendSV_Handler                  /*14: PendSV                  */
    .word   SysTick_Handler                 /*15: SysTick                 */
    /* STM32G071 peripheral interrupts */
    .word   WWDG_IRQHandler                 /*  0 */
    .word   PVD_IRQHandler                  /*  1 */
    .word   RTC_TAMP_IRQHandler             /*  2 */
    .word   FLASH_IRQHandler                /*  3 */
    .word   RCC_IRQHandler                  /*  4 */
    .word   EXTI0_1_IRQHandler              /*  5 */
    .word   EXTI2_3_IRQHandler              /*  6 */
    .word   EXTI4_15_IRQHandler             /*  7 */
    .word   UCPD1_2_IRQHandler              /*  8 */
    .word   DMA1_Channel1_IRQHandler        /*  9 */
    .word   DMA1_Channel2_3_IRQHandler      /* 10 */
    .word   DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler /* 11 */
    .word   ADC1_COMP_IRQHandler            /* 12 */
    .word   TIM1_BRK_UP_TRG_COM_IRQHandler /* 13 */
    .word   TIM1_CC_IRQHandler              /* 14 */
    .word   TIM2_IRQHandler                 /* 15 */
    .word   TIM3_IRQHandler                 /* 16 */
    .word   TIM6_DAC_LPTIM1_IRQHandler      /* 17 */
    .word   TIM7_LPTIM2_IRQHandler          /* 18 */
    .word   TIM14_IRQHandler                /* 19 */
    .word   TIM15_IRQHandler                /* 20 */
    .word   TIM16_IRQHandler                /* 21 */
    .word   TIM17_IRQHandler                /* 22 */
    .word   I2C1_IRQHandler                 /* 23 */
    .word   I2C2_IRQHandler                 /* 24 */
    .word   SPI1_IRQHandler                 /* 25 */
    .word   SPI2_IRQHandler                 /* 26 */
    .word   USART1_IRQHandler               /* 27 */
    .word   USART2_IRQHandler               /* 28 */
    .word   USART3_4_LPUART1_IRQHandler     /* 29 */
    .word   CEC_IRQHandler                  /* 30 */
    .word   AES_RNG_IRQHandler              /* 31 */

/* =========================================================== */
/*  Reset Handler                                               */
/* =========================================================== */
    .section .text.Reset_Handler, "ax", %progbits
    .global Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    /* Copy .data section from Flash to SRAM */
    ldr     r0, =__data_start_flash
    ldr     r1, =__data_start
    ldr     r2, =__data_end
    cmp     r1, r2
    beq     .L_data_done
.L_copy_data:
    ldm     r0!, {r3}
    stm     r1!, {r3}
    cmp     r1, r2
    blo     .L_copy_data
.L_data_done:

    /* Zero .bss section */
    ldr     r0, =__bss_start
    ldr     r1, =__bss_end
    movs    r2, #0
    cmp     r0, r1
    beq     .L_bss_done
.L_zero_bss:
    stm     r0!, {r2}
    cmp     r0, r1
    blo     .L_zero_bss
.L_bss_done:

    /* Call SystemInit (clock at reset default) */
    bl      SystemInit

    /* Jump to main */
    bl      main

    /* Should never return — spin forever */
.L_loop_forever:
    b       .L_loop_forever
    .size Reset_Handler, .-Reset_Handler

/* =========================================================== */
/*  Default / Weak IRQ Handlers                                 */
/* =========================================================== */
    .section .text, "ax", %progbits

    .macro DEFAULT_HANDLER name
    .weak \name
    .thumb_set \name, Default_Handler
    .endm

    .type Default_Handler, %function
Default_Handler:
    b   Default_Handler
    .size Default_Handler, .-Default_Handler

    DEFAULT_HANDLER NMI_Handler
    DEFAULT_HANDLER HardFault_Handler
    DEFAULT_HANDLER SVC_Handler
    DEFAULT_HANDLER PendSV_Handler
    DEFAULT_HANDLER SysTick_Handler

    DEFAULT_HANDLER WWDG_IRQHandler
    DEFAULT_HANDLER PVD_IRQHandler
    DEFAULT_HANDLER RTC_TAMP_IRQHandler
    DEFAULT_HANDLER FLASH_IRQHandler
    DEFAULT_HANDLER RCC_IRQHandler
    DEFAULT_HANDLER EXTI0_1_IRQHandler
    DEFAULT_HANDLER EXTI2_3_IRQHandler
    DEFAULT_HANDLER EXTI4_15_IRQHandler
    DEFAULT_HANDLER UCPD1_2_IRQHandler
    DEFAULT_HANDLER DMA1_Channel1_IRQHandler
    DEFAULT_HANDLER DMA1_Channel2_3_IRQHandler
    DEFAULT_HANDLER DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler
    DEFAULT_HANDLER ADC1_COMP_IRQHandler
    DEFAULT_HANDLER TIM1_BRK_UP_TRG_COM_IRQHandler
    DEFAULT_HANDLER TIM1_CC_IRQHandler
    DEFAULT_HANDLER TIM2_IRQHandler
    DEFAULT_HANDLER TIM3_IRQHandler
    DEFAULT_HANDLER TIM6_DAC_LPTIM1_IRQHandler
    DEFAULT_HANDLER TIM7_LPTIM2_IRQHandler
    DEFAULT_HANDLER TIM14_IRQHandler
    DEFAULT_HANDLER TIM15_IRQHandler
    DEFAULT_HANDLER TIM16_IRQHandler
    DEFAULT_HANDLER TIM17_IRQHandler
    DEFAULT_HANDLER I2C1_IRQHandler
    DEFAULT_HANDLER I2C2_IRQHandler
    DEFAULT_HANDLER SPI1_IRQHandler
    DEFAULT_HANDLER SPI2_IRQHandler
    DEFAULT_HANDLER USART1_IRQHandler
    DEFAULT_HANDLER USART2_IRQHandler
    DEFAULT_HANDLER USART3_4_LPUART1_IRQHandler
    DEFAULT_HANDLER CEC_IRQHandler
    DEFAULT_HANDLER AES_RNG_IRQHandler

/* =========================================================== */
/*  Heap / Stack symbols for newlib (sbrk)                      */
/* =========================================================== */
    .global __HeapBase
    .global __HeapLimit
    .global __StackTop
