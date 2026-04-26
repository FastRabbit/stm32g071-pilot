/**
 * @file core_cm0plus.h
 * @brief CMSIS Cortex-M0+ Core Peripheral Access Layer (minimal stub).
 *
 * In production, replace this with the official ARM CMSIS core file from:
 *   CMSIS/Core/Include/core_cm0plus.h
 *
 * This stub provides the subset of definitions needed to compile the
 * application without the full CMSIS package present.
 */
#ifndef CORE_CM0PLUS_H
#define CORE_CM0PLUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Volatile qualifier used by CMSIS for memory-mapped registers */
#ifndef __IO
#define __IO  volatile
#endif
#ifndef __I
#define __I   volatile const
#endif
#ifndef __O
#define __O   volatile
#endif

/* =========================================================== */
/*              NVIC                                             */
/* =========================================================== */
typedef struct {
    volatile uint32_t ISER[1U];
    uint32_t          RESERVED0[31U];
    volatile uint32_t ICER[1U];
    uint32_t          RESERVED1[31U];
    volatile uint32_t ISPR[1U];
    uint32_t          RESERVED2[31U];
    volatile uint32_t ICPR[1U];
    uint32_t          RESERVED3[31U];
    uint32_t          RESERVED4[64U];
    volatile uint32_t IP[8U];
} NVIC_Type;

/* =========================================================== */
/*              SysTick                                          */
/* =========================================================== */
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_Type;

#define SysTick_CTRL_CLKSOURCE_Pos  (2U)
#define SysTick_CTRL_CLKSOURCE_Msk  (1UL << SysTick_CTRL_CLKSOURCE_Pos)
#define SysTick_CTRL_TICKINT_Pos    (1U)
#define SysTick_CTRL_TICKINT_Msk    (1UL << SysTick_CTRL_TICKINT_Pos)
#define SysTick_CTRL_ENABLE_Pos     (0U)
#define SysTick_CTRL_ENABLE_Msk     (1UL << SysTick_CTRL_ENABLE_Pos)
#define SysTick_CTRL_COUNTFLAG_Pos  (16U)
#define SysTick_CTRL_COUNTFLAG_Msk  (1UL << SysTick_CTRL_COUNTFLAG_Pos)

/* =========================================================== */
/*              SCB                                              */
/* =========================================================== */
typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    uint32_t          RESERVED0[1U];
    volatile uint32_t SHP[2U];
    volatile uint32_t SHCSR;
} SCB_Type;

/* =========================================================== */
/*              Core Peripheral Base Addresses                   */
/* =========================================================== */
#define SCS_BASE    (0xE000E000UL)
#define SysTick_BASE (SCS_BASE + 0x0010UL)
#define NVIC_BASE   (SCS_BASE + 0x0100UL)
#define SCB_BASE    (SCS_BASE + 0x0D00UL)

#define SysTick     ((SysTick_Type *)SysTick_BASE)
#define NVIC        ((NVIC_Type *)NVIC_BASE)
#define SCB         ((SCB_Type *)SCB_BASE)

/* =========================================================== */
/*              Inline Core Functions                            */
/* =========================================================== */
__attribute__((always_inline)) static inline void __enable_irq(void)
{
    __asm volatile ("cpsie i" : : : "memory");
}

__attribute__((always_inline)) static inline void __disable_irq(void)
{
    __asm volatile ("cpsid i" : : : "memory");
}

__attribute__((always_inline)) static inline void __NOP(void)
{
    __asm volatile ("nop");
}

__attribute__((always_inline)) static inline void __WFI(void)
{
    __asm volatile ("wfi" : : : "memory");
}

static inline void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC->ISER[0U] = (uint32_t)(1UL << ((uint32_t)IRQn & 0x1FUL));
    }
}

static inline void NVIC_DisableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)IRQn >= 0) {
        NVIC->ICER[0U] = (uint32_t)(1UL << ((uint32_t)IRQn & 0x1FUL));
    }
}

static inline uint32_t SysTick_Config(uint32_t ticks)
{
    if ((ticks - 1UL) > 0x00FFFFFFUL) {
        return 1UL; /* reload value impossible */
    }
    SysTick->LOAD  = (uint32_t)(ticks - 1UL);
    SysTick->VAL   = 0UL;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk    |
                     SysTick_CTRL_ENABLE_Msk;
    return 0UL;
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_CM0PLUS_H */
