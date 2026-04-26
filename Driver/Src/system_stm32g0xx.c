/**
 * @file system_stm32g0xx.c
 * @brief System clock configuration for STM32G071 @ 64 MHz.
 *
 * Clock path:  HSI16 (16 MHz)  →  PLL  (×8 / ÷2)  →  SYSCLK 64 MHz
 * AHB  = 64 MHz  (HPRE  = /1)
 * APB1 = 64 MHz  (PPRE  = /1)
 */
#include "stm32g0xx.h"
#include "system_stm32g0xx.h"

uint32_t SystemCoreClock = 16000000UL; /* updated by SystemCoreClockUpdate */

void SystemInit(void)
{
    /* Nothing needed: reset values are safe for 16 MHz HSI */
}

/**
 * @brief  Configure system clock to 64 MHz via PLL fed from HSI16.
 *
 *   PLLSRC = HSI16  (16 MHz)
 *   PLLM   = /1
 *   PLLN   = ×8    → VCO = 128 MHz
 *   PLLR   = /2    → PLL_R = 64 MHz  (SYSCLK)
 */
void SystemClock_Config(void)
{
    /* 1. Set 2 wait states for Flash at 64 MHz */
    MODIFY_REG(FLASH->ACR,
               FLASH_ACR_LATENCY_Msk,
               FLASH_ACR_LATENCY_2WS);

    /* 2. Make sure HSI16 is ON and ready */
    SET_BIT(RCC->CR, RCC_CR_HSION);
    while (!READ_BIT(RCC->CR, RCC_CR_HSIRDY)) { /* wait */ }

    /* 3. Configure PLL: source = HSI16, M=/1, N=×8, R=/2 */
    MODIFY_REG(RCC->PLLCFGR,
               RCC_PLLCFGR_PLLSRC_Msk |
               RCC_PLLCFGR_PLLM_Msk   |
               RCC_PLLCFGR_PLLN_Msk   |
               RCC_PLLCFGR_PLLR_Msk   |
               RCC_PLLCFGR_PLLREN,
               RCC_PLLCFGR_PLLSRC_HSI |
               (0UL << RCC_PLLCFGR_PLLM_Pos) |   /* M = 1 → 0b000 */
               (8UL << RCC_PLLCFGR_PLLN_Pos) |   /* N = 8          */
               (0UL << RCC_PLLCFGR_PLLR_Pos) |   /* R = 2 → 0b000  */
               RCC_PLLCFGR_PLLREN);

    /* 4. Enable PLL and wait until ready */
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (!READ_BIT(RCC->CR, RCC_CR_PLLRDY)) { /* wait */ }

    /* 5. Switch SYSCLK to PLL */
    MODIFY_REG(RCC->CFGR,
               RCC_CFGR_SW_Msk,
               RCC_CFGR_SW_PLL);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL) { /* wait */ }

    SystemCoreClock = 64000000UL;
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = 64000000UL;
}
