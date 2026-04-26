/**
 * @file main.c
 * @brief STM32G071 superloop — LED blink using TIM6 delay.
 *
 * Boot sequence:
 *   1. Configure system clock → 64 MHz via PLL
 *   2. Enable GPIO-A clock and configure PA5 as output (LED)
 *   3. Initialise TIM6 for millisecond delays
 *   4. Enter while(1) toggle loop at 500 ms cadence
 */
#include "main.h"
#include "system_stm32g0xx.h"
#include "uart1.h"
#include "uart2.h"
#include "spi1.h"
#include "spi2.h"
#include "fdcan1.h"
#include <cstdio>

/* =========================================================== */
/*  Forward declarations                                         */
/* =========================================================== */
static void GPIO_Init(void);
static void TIM6_Init(void);
static void delay_ms(uint32_t ms);

/* =========================================================== */
/*  Entry point                                                  */
/* =========================================================== */
int main(void)
{
    SystemClock_Config();
    UART2_Init();
    UART1_Init();
    GPIO_Init();
    TIM6_Init();
    SPI1_Init();
    SPI2_Init();
    FDCAN1_Init();

    printf("[boot] STM32G0B1 ready — UART2 @ 115200 8N1\r\n");
    printf("[boot] SPI1 (IRQ)  PB3/PB4/PB5 ready\r\n");
    printf("[boot] SPI2 (DMA)  PB13/PB14/PB15 ready\r\n");

    static uint32_t tick = 0U;

    while (1) {
        /* Toggle LED */
        GPIOA->ODR ^= (1UL << LED_GPIO_PIN);
        printf("[%lu] LED toggle\r\n", tick++);
        delay_ms(500);
    }

    /* Unreachable */
    return 0;
}

/* =========================================================== */
/*  GPIO Initialisation — PA5 push-pull output (LED)            */
/* =========================================================== */
static void GPIO_Init(void)
{
    /* Enable GPIOA clock */
    LED_GPIO_CLK_EN();

    /* PA5: MODER = 01 (general-purpose output) */
    MODIFY_REG(GPIOA->MODER,
               (0x3UL << (LED_GPIO_PIN * 2U)),
               (0x1UL << (LED_GPIO_PIN * 2U)));

    /* PA5: OTYPER = 0 (push-pull) — reset default, written explicitly */
    CLEAR_BIT(GPIOA->OTYPER, (1UL << LED_GPIO_PIN));

    /* PA5: OSPEEDR = 00 (low speed) */
    CLEAR_BIT(GPIOA->OSPEEDR, (0x3UL << (LED_GPIO_PIN * 2U)));

    /* PA5: PUPDR = 00 (no pull) */
    CLEAR_BIT(GPIOA->PUPDR, (0x3UL << (LED_GPIO_PIN * 2U)));
}

/* =========================================================== */
/*  TIM6 Initialisation — 1 ms tick @ 64 MHz                   */
/*                                                               */
/*  PSC = 63  → timer clock = 64 MHz / 64 = 1 MHz              */
/*  ARR = 999 → update event every 1000 counts = 1 ms           */
/* =========================================================== */
static void TIM6_Init(void)
{
    /* Enable TIM6 clock on APB1 */
    SET_BIT(RCC->APBENR1, RCC_APBENR1_TIM6EN);

    TIM6->CR  = 0U;
    TIM6->PSC  = 63U;      /* /64 → 1 MHz */
    TIM6->ARR  = 999U;     /* count to 1000 → 1 ms */
    TIM6->EGR  = TIM_EGR_UG; /* force update to load PSC/ARR */
    TIM6->SR   = 0U;       /* clear UIF */
}

/* =========================================================== */
/*  Blocking millisecond delay using TIM6                       */
/* =========================================================== */
static void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        /* Start one 1 ms count */
        TIM6->CNT = 0U;
        TIM6->SR  = 0U;
        SET_BIT(TIM6->CR, TIM_CR_CEN);

        /* Wait for update flag */
        while (!READ_BIT(TIM6->SR, TIM_SR_UIF)) { /* spin */ }

        /* Stop timer */
        CLEAR_BIT(TIM6->CR, TIM_CR_CEN);
        TIM6->SR = 0U;
    }
}
