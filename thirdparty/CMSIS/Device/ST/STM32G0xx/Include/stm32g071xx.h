/**
 * @file stm32g071xx.h
 * @brief STM32G071xx device peripheral register definitions (minimal stub).
 *
 * In a production project this file is taken verbatim from the STM32Cube
 * firmware package:
 *   Drivers/CMSIS/Device/ST/STM32G0xx/Include/stm32g071xx.h
 *
 * For this scaffold we provide the register-level definitions needed to
 * compile the LL driver and application code without the full package.
 */
#ifndef STM32G071XX_H
#define STM32G071XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* =========================================================== */
/*              Interrupt Number Definition                      */
/* =========================================================== */
typedef enum {
    /* Cortex-M0+ internal exceptions */
    NonMaskableInt_IRQn    = -14,
    HardFault_IRQn         = -13,
    SVCall_IRQn            = -5,
    PendSV_IRQn            = -2,
    SysTick_IRQn           = -1,
    /* STM32G071 peripheral interrupts (subset) */
    WWDG_IRQn              = 0,
    RCC_IRQn               = 4,
    EXTI0_1_IRQn           = 5,
    EXTI2_3_IRQn           = 6,
    EXTI4_15_IRQn          = 7,
    DMA1_Channel1_IRQn     = 9,
    ADC1_IRQn              = 12,
    TIM1_BRK_UP_TRG_COM_IRQn = 13,
    TIM1_CC_IRQn           = 14,
    TIM2_IRQn              = 15,
    TIM3_IRQn              = 16,
    TIM6_DAC_LPTIM1_IRQn   = 17,
    TIM7_LPTIM2_IRQn       = 18,
    TIM14_IRQn             = 19,
    TIM15_IRQn             = 20,
    TIM16_IRQn             = 21,
    TIM17_IRQn             = 22,
    I2C1_IRQn              = 23,
    I2C2_IRQn              = 24,
    SPI1_IRQn              = 25,
    SPI2_IRQn              = 26,
    USART1_IRQn            = 27,
    USART2_IRQn            = 28,
    USART3_USART4_LPUART1_IRQn = 29,
    CEC_IRQn               = 30,
    AES_RNG_IRQn           = 31,
} IRQn_Type;

/* =========================================================== */
/*              Processor and Core Peripheral Section            */
/* =========================================================== */
#define __CM0PLUS_REV             0U
#define __MPU_PRESENT             1U
#define __VTOR_PRESENT            1U
#define __NVIC_PRIO_BITS          2U
#define __Vendor_SysTickConfig    0U

#include "core_cm0plus.h"

/* =========================================================== */
/*              Peripheral Register Structures                   */
/* =========================================================== */

/* -- RCC -- */
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t ICSCR;
    volatile uint32_t CFGR;
    volatile uint32_t PLLCFGR;
    uint32_t          RESERVED0[2];
    volatile uint32_t CIER;
    volatile uint32_t CIFR;
    volatile uint32_t CICR;
    volatile uint32_t IOPRSTR;
    volatile uint32_t AHBRSTR;
    volatile uint32_t APBRSTR1;
    volatile uint32_t APBRSTR2;
    volatile uint32_t IOPENR;
    volatile uint32_t AHBENR;
    volatile uint32_t APBENR1;
    volatile uint32_t APBENR2;
    volatile uint32_t IOPSMENR;
    volatile uint32_t AHBSMENR;
    volatile uint32_t APBSMENR1;
    volatile uint32_t APBSMENR2;
    volatile uint32_t CCIPR;
    volatile uint32_t CCIPR2;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_TypeDef;

/* -- GPIO -- */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
    volatile uint32_t BRR;
} GPIO_TypeDef;

/* -- USART -- */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t GTPR;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
    volatile uint32_t PRESC;
} USART_TypeDef;

/* -- SPI -- */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CRCPR;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
} SPI_TypeDef;

/* -- TIM (basic/general) -- */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

/* -- FLASH -- */
typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t RESERVED0;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t ECCR;
    volatile uint32_t RESERVED1;
    volatile uint32_t OPTR;
    volatile uint32_t PCROP1ASR;
    volatile uint32_t PCROP1AER;
    volatile uint32_t WRP1AR;
    volatile uint32_t WRP1BR;
    volatile uint32_t PCROP1BSR;
    volatile uint32_t PCROP1BER;
} FLASH_TypeDef;

/* =========================================================== */
/*              Peripheral Memory Map                            */
/* =========================================================== */
#define FLASH_BASE          0x08000000UL
#define SRAM_BASE           0x20000000UL
#define PERIPH_BASE         0x40000000UL

#define IOPORT_BASE         0x50000000UL
#define GPIOA_BASE          (IOPORT_BASE + 0x00000000UL)
#define GPIOB_BASE          (IOPORT_BASE + 0x00000400UL)
#define GPIOC_BASE          (IOPORT_BASE + 0x00000800UL)
#define GPIOD_BASE          (IOPORT_BASE + 0x00000C00UL)
#define GPIOF_BASE          (IOPORT_BASE + 0x00001400UL)

#define APBPERIPH_BASE      PERIPH_BASE
#define TIM2_BASE           (APBPERIPH_BASE + 0x00000000UL)
#define TIM3_BASE           (APBPERIPH_BASE + 0x00000400UL)
#define TIM6_BASE           (APBPERIPH_BASE + 0x00001000UL)
#define TIM7_BASE           (APBPERIPH_BASE + 0x00001400UL)
#define TIM14_BASE          (APBPERIPH_BASE + 0x00002000UL)
#define SPI2_BASE           (APBPERIPH_BASE + 0x00003800UL)
#define USART2_BASE         (APBPERIPH_BASE + 0x00004400UL)
#define USART3_BASE         (APBPERIPH_BASE + 0x00004800UL)
#define USART4_BASE         (APBPERIPH_BASE + 0x00004C00UL)
#define I2C1_BASE           (APBPERIPH_BASE + 0x00005400UL)
#define I2C2_BASE           (APBPERIPH_BASE + 0x00005800UL)

#define APBPERIPH2_BASE     (PERIPH_BASE + 0x00010000UL)
#define SYSCFG_BASE         (APBPERIPH2_BASE + 0x00000000UL)
#define TIM1_BASE           (APBPERIPH2_BASE + 0x00002C00UL)
#define SPI1_BASE           (APBPERIPH2_BASE + 0x00003000UL)
#define USART1_BASE         (APBPERIPH2_BASE + 0x00003800UL)
#define TIM15_BASE          (APBPERIPH2_BASE + 0x00004000UL)
#define TIM16_BASE          (APBPERIPH2_BASE + 0x00004400UL)
#define TIM17_BASE          (APBPERIPH2_BASE + 0x00004800UL)

#define AHBPERIPH_BASE      (PERIPH_BASE + 0x00020000UL)
#define FLASH_R_BASE        (AHBPERIPH_BASE + 0x00002000UL)
#define RCC_BASE            (AHBPERIPH_BASE + 0x00001000UL)

/* =========================================================== */
/*              Peripheral Declarations                          */
/* =========================================================== */
#define GPIOA   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOF   ((GPIO_TypeDef *)GPIOF_BASE)

#define RCC     ((RCC_TypeDef *)RCC_BASE)
#define FLASH   ((FLASH_TypeDef *)FLASH_R_BASE)

#define TIM1    ((TIM_TypeDef *)TIM1_BASE)
#define TIM2    ((TIM_TypeDef *)TIM2_BASE)
#define TIM3    ((TIM_TypeDef *)TIM3_BASE)
#define TIM6    ((TIM_TypeDef *)TIM6_BASE)
#define TIM7    ((TIM_TypeDef *)TIM7_BASE)
#define TIM14   ((TIM_TypeDef *)TIM14_BASE)
#define TIM15   ((TIM_TypeDef *)TIM15_BASE)
#define TIM16   ((TIM_TypeDef *)TIM16_BASE)
#define TIM17   ((TIM_TypeDef *)TIM17_BASE)

#define USART1  ((USART_TypeDef *)USART1_BASE)
#define USART2  ((USART_TypeDef *)USART2_BASE)
#define USART3  ((USART_TypeDef *)USART3_BASE)
#define USART4  ((USART_TypeDef *)USART4_BASE)

#define SPI1    ((SPI_TypeDef *)SPI1_BASE)
#define SPI2    ((SPI_TypeDef *)SPI2_BASE)

/* =========================================================== */
/*              Bit Definitions (subset used by LL drivers)      */
/* =========================================================== */

/* RCC_CR */
#define RCC_CR_HSION_Pos        (8U)
#define RCC_CR_HSION            (1UL << RCC_CR_HSION_Pos)
#define RCC_CR_HSIRDY_Pos       (10U)
#define RCC_CR_HSIRDY           (1UL << RCC_CR_HSIRDY_Pos)
#define RCC_CR_HSITRIM_Pos      (11U)
#define RCC_CR_HSITRIM_Msk      (0x7FUL << RCC_CR_HSITRIM_Pos)
#define RCC_CR_PLLON_Pos        (24U)
#define RCC_CR_PLLON            (1UL << RCC_CR_PLLON_Pos)
#define RCC_CR_PLLRDY_Pos       (25U)
#define RCC_CR_PLLRDY           (1UL << RCC_CR_PLLRDY_Pos)

/* RCC_CFGR */
#define RCC_CFGR_SW_Pos         (0U)
#define RCC_CFGR_SW_Msk         (0x7UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_PLL         (0x2UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos        (3U)
#define RCC_CFGR_SWS_Msk        (0x7UL << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_PLL        (0x2UL << RCC_CFGR_SWS_Pos)

/* RCC_PLLCFGR */
#define RCC_PLLCFGR_PLLSRC_Pos  (0U)
#define RCC_PLLCFGR_PLLSRC_Msk  (0x3UL << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLSRC_HSI  (0x2UL << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLM_Pos    (4U)
#define RCC_PLLCFGR_PLLM_Msk    (0x7UL << RCC_PLLCFGR_PLLM_Pos)
#define RCC_PLLCFGR_PLLN_Pos    (8U)
#define RCC_PLLCFGR_PLLN_Msk    (0x7FUL << RCC_PLLCFGR_PLLN_Pos)
#define RCC_PLLCFGR_PLLR_Pos    (29U)
#define RCC_PLLCFGR_PLLR_Msk    (0x7UL << RCC_PLLCFGR_PLLR_Pos)
#define RCC_PLLCFGR_PLLREN_Pos  (28U)
#define RCC_PLLCFGR_PLLREN      (1UL << RCC_PLLCFGR_PLLREN_Pos)

/* RCC_IOPENR */
#define RCC_IOPENR_GPIOAEN_Pos  (0U)
#define RCC_IOPENR_GPIOAEN      (1UL << RCC_IOPENR_GPIOAEN_Pos)
#define RCC_IOPENR_GPIOBEN_Pos  (1U)
#define RCC_IOPENR_GPIOBEN      (1UL << RCC_IOPENR_GPIOBEN_Pos)
#define RCC_IOPENR_GPIOCEN_Pos  (2U)
#define RCC_IOPENR_GPIOCEN      (1UL << RCC_IOPENR_GPIOCEN_Pos)

/* RCC_APBENR1 */
#define RCC_APBENR1_TIM2EN_Pos  (0U)
#define RCC_APBENR1_TIM2EN      (1UL << RCC_APBENR1_TIM2EN_Pos)
#define RCC_APBENR1_USART2EN_Pos (17U)
#define RCC_APBENR1_USART2EN    (1UL << RCC_APBENR1_USART2EN_Pos)
#define RCC_APBENR1_SPI2EN_Pos  (14U)
#define RCC_APBENR1_SPI2EN      (1UL << RCC_APBENR1_SPI2EN_Pos)

/* RCC_APBENR2 */
#define RCC_APBENR2_USART1EN_Pos (14U)
#define RCC_APBENR2_USART1EN    (1UL << RCC_APBENR2_USART1EN_Pos)
#define RCC_APBENR2_SPI1EN_Pos  (12U)
#define RCC_APBENR2_SPI1EN      (1UL << RCC_APBENR2_SPI1EN_Pos)
#define RCC_APBENR2_TIM1EN_Pos  (11U)
#define RCC_APBENR2_TIM1EN      (1UL << RCC_APBENR2_TIM1EN_Pos)

/* GPIO_MODER */
#define GPIO_MODER_MODE0_Pos    (0U)
#define GPIO_MODER_MODE0_Msk    (0x3UL << GPIO_MODER_MODE0_Pos)

/* USART_CR1 */
#define USART_CR1_UE_Pos        (0U)
#define USART_CR1_UE            (1UL << USART_CR1_UE_Pos)
#define USART_CR1_RE_Pos        (2U)
#define USART_CR1_RE            (1UL << USART_CR1_RE_Pos)
#define USART_CR1_TE_Pos        (3U)
#define USART_CR1_TE            (1UL << USART_CR1_TE_Pos)
#define USART_CR1_RXNEIE_Pos    (5U)
#define USART_CR1_RXNEIE        (1UL << USART_CR1_RXNEIE_Pos)
#define USART_CR1_M0_Pos        (12U)
#define USART_CR1_M0            (1UL << USART_CR1_M0_Pos)
#define USART_CR1_OVER8_Pos     (15U)
#define USART_CR1_OVER8         (1UL << USART_CR1_OVER8_Pos)

/* USART_ISR */
#define USART_ISR_RXNE_RXFNE_Pos (5U)
#define USART_ISR_RXNE_RXFNE    (1UL << USART_ISR_RXNE_RXFNE_Pos)
#define USART_ISR_TC_Pos        (6U)
#define USART_ISR_TC            (1UL << USART_ISR_TC_Pos)
#define USART_ISR_TXE_TXFNF_Pos (7U)
#define USART_ISR_TXE_TXFNF     (1UL << USART_ISR_TXE_TXFNF_Pos)

/* SPI_CR1 */
#define SPI_CR1_CPHA_Pos        (0U)
#define SPI_CR1_CPHA            (1UL << SPI_CR1_CPHA_Pos)
#define SPI_CR1_CPOL_Pos        (1U)
#define SPI_CR1_CPOL            (1UL << SPI_CR1_CPOL_Pos)
#define SPI_CR1_MSTR_Pos        (2U)
#define SPI_CR1_MSTR            (1UL << SPI_CR1_MSTR_Pos)
#define SPI_CR1_BR_Pos          (3U)
#define SPI_CR1_BR_Msk          (0x7UL << SPI_CR1_BR_Pos)
#define SPI_CR1_SPE_Pos         (6U)
#define SPI_CR1_SPE             (1UL << SPI_CR1_SPE_Pos)
#define SPI_CR1_SSI_Pos         (8U)
#define SPI_CR1_SSI             (1UL << SPI_CR1_SSI_Pos)
#define SPI_CR1_SSM_Pos         (9U)
#define SPI_CR1_SSM             (1UL << SPI_CR1_SSM_Pos)

/* SPI_SR */
#define SPI_SR_RXNE_Pos         (0U)
#define SPI_SR_RXNE             (1UL << SPI_SR_RXNE_Pos)
#define SPI_SR_TXE_Pos          (1U)
#define SPI_SR_TXE              (1UL << SPI_SR_TXE_Pos)
#define SPI_SR_BSY_Pos          (7U)
#define SPI_SR_BSY              (1UL << SPI_SR_BSY_Pos)

/* SPI_CR2 */
#define SPI_CR2_DS_Pos          (8U)
#define SPI_CR2_DS_Msk          (0xFUL << SPI_CR2_DS_Pos)
#define SPI_CR2_FRXTH_Pos       (12U)
#define SPI_CR2_FRXTH           (1UL << SPI_CR2_FRXTH_Pos)

/* TIM_CR1 */
#define TIM_CR1_CEN_Pos         (0U)
#define TIM_CR1_CEN             (1UL << TIM_CR1_CEN_Pos)
#define TIM_CR1_URS_Pos         (2U)
#define TIM_CR1_URS             (1UL << TIM_CR1_URS_Pos)

/* TIM_SR */
#define TIM_SR_UIF_Pos          (0U)
#define TIM_SR_UIF              (1UL << TIM_SR_UIF_Pos)

/* TIM_EGR */
#define TIM_EGR_UG_Pos          (0U)
#define TIM_EGR_UG              (1UL << TIM_EGR_UG_Pos)

/* FLASH_ACR */
#define FLASH_ACR_LATENCY_Pos   (0U)
#define FLASH_ACR_LATENCY_Msk   (0x7UL << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_LATENCY_2WS   (0x2UL << FLASH_ACR_LATENCY_Pos)

/* =========================================================== */
/*              Utility Macros                                   */
/* =========================================================== */
#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)      ((REG) & (BIT))
#define CLEAR_REG(REG)          ((REG) = 0U)
#define WRITE_REG(REG, VAL)     ((REG) = (VAL))
#define READ_REG(REG)           ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) \
    WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#ifdef __cplusplus
}
#endif

#endif /* STM32G071XX_H */
