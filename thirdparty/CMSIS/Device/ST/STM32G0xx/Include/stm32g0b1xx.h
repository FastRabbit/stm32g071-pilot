/**
 * @file stm32g0b1xx.h
 * @brief STM32G0B1xx device header — minimal peripheral stubs for LL driver.
 *
 * Device: STM32G0B1RET6
 *   Flash: 512 KB @ 0x08000000
 *   SRAM:  128 KB @ 0x20000000
 *
 * Peripherals: USART1/2, SPI1/2, TIM6, FDCAN1/FDCAN2, DMA, GPIO, RCC, etc.
 * See STM32G0B1 reference manual for register details.
 */
#ifndef STM32G0B1XX_H
#define STM32G0B1XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ========================================================================= */
/*  Volatile access macros (from CMSIS)                                     */
/* ========================================================================= */

#define __IO    volatile
#define __I     volatile const
#define __O     volatile

/* Bit manipulation helpers */
#define SET_BIT(REG, BIT)         ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)       ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)        ((REG) & (BIT))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) \
        ((REG) = (((REG) & (~(CLEARMASK))) | (SETMASK)))
#define WRITE_REG(REG, VAL)       ((REG) = (VAL))
#define READ_REG(REG)             ((REG))
#define CLEAR_REG(REG)            ((REG) = 0x0UL)

typedef struct {
    __IO uint32_t ACR;
} FLASH_TypeDef;

typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t CRCPR;
    __IO uint32_t RXCRCR;
    __IO uint32_t TXCRCR;
    __IO uint32_t I2SCFGR;
} SPI_TypeDef;

typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t CR3;
    __IO uint32_t BRR;
    __IO uint32_t GTPR;
    __IO uint32_t RTOR;
    __IO uint32_t RQR;
    __IO uint32_t ISR;
    __IO uint32_t ICR;
    __IO uint32_t RDR;
    __IO uint32_t TDR;
    __IO uint32_t PRESC;
} USART_TypeDef;

typedef struct {
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
    __IO uint32_t BRR;
    __IO uint32_t HSLVR;
} GPIO_TypeDef;

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t CFGR;
    __IO uint32_t CIER;
    __IO uint32_t CIFR;
    __IO uint32_t PLLCFGR;
    uint32_t      RESERVED0;
    __IO uint32_t CRRCR;
    __IO uint32_t CCIPR;
    uint32_t      RESERVED1;
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
    __IO uint32_t IOPENR;
    __IO uint32_t AHBENR;
    __IO uint32_t APBENR1;
    __IO uint32_t APBENR2;
    __IO uint32_t IOPSMENR;
    __IO uint32_t AHBSMENR;
    __IO uint32_t APBSMENR1;
    __IO uint32_t APBSMENR2;
} RCC_TypeDef;

typedef struct {
    __IO uint32_t ISR;
    __IO uint32_t IFCR;
    __IO uint32_t CCR;
    __IO uint32_t CNDTR;
    __IO uint32_t CPAR;
    __IO uint32_t CMAR;
} DMA_Channel_TypeDef;

typedef struct {
    __IO uint32_t ISR;
    __IO uint32_t IFCR;
} DMA_TypeDef;

typedef struct {
    __IO uint32_t CCR;
} DMAMUX_Channel_TypeDef;

typedef struct {
    __IO uint32_t CREL;         /* Version */
    __IO uint32_t ENDN;         /* Endianness */
    uint32_t      RESERVED0[1];
    __IO uint32_t CCCR;         /* Clock config / control */
    __IO uint32_t NBTP;         /* Nominal bitrate */
    __IO uint32_t TSYNC;        /* Timestamp sync */
    __IO uint32_t ECRC;         /* Error correction */
    __IO uint32_t PSR;          /* Protocol status */
    __IO uint32_t TDCR;         /* Transmitter delay compensation */
    uint32_t      RESERVED1[1];
    __IO uint32_t IR;           /* Interrupt register */
    __IO uint32_t IE;           /* Interrupt enable */
    __IO uint32_t ILS;          /* Interrupt line select */
    __IO uint32_t ILE;          /* Interrupt line enable */
    uint32_t      RESERVED2[8];
    __IO uint32_t RXGPC;        /* RX global process control */
    __IO uint32_t RXFIS;        /* RX filter index setup */
    __IO uint32_t RXFI0;        /* RX FIFO 0 */
    __IO uint32_t RXFI1;        /* RX FIFO 1 */
    uint32_t      RESERVED3[8];
    __IO uint32_t RXESC;        /* RX element size config */
    __IO uint32_t TXESC;        /* TX element size config */
    __IO uint32_t TXBRP;        /* TX buffer request pending */
    __IO uint32_t TXFQS;        /* TX FIFO queue status */
    __IO uint32_t TXEFC;        /* TX event FIFO config */
} FDCAN_TypeDef;

typedef struct {
    __IO uint32_t CREL;
    __IO uint32_t ENDN;
    uint32_t      RESERVED1;
    __IO uint32_t DBTP;
    __IO uint32_t TEST;
    __IO uint32_t RWD;
    __IO uint32_t CCCR;
    __IO uint32_t NBTP;
    __IO uint32_t TSYNC;
    __IO uint32_t ECRC;
    __IO uint32_t PSR;
    __IO uint32_t TDCR;
    uint32_t      RESERVED2;
    __IO uint32_t IR;
    __IO uint32_t IE;
    __IO uint32_t ILS;
    __IO uint32_t ILE;
    uint32_t      RESERVED3[8];
    __IO uint32_t RXGPC;
    __IO uint32_t RXFIS;
    __IO uint32_t RXFI0;
    __IO uint32_t RXFI1;
    uint32_t      RESERVED4[8];
    __IO uint32_t RXESC;
    __IO uint32_t TXESC;
    __IO uint32_t TXBRP;
    __IO uint32_t TXFQS;
    __IO uint32_t TXESC_2;
    __IO uint32_t TXEFC;
} FDCAN_GlobalTypeDef;

typedef struct {
    __IO uint32_t CCCR;
    __IO uint32_t NBTP;
    __IO uint32_t TSYNC;
    __IO uint32_t ECRC;
    __IO uint32_t PSR;
    __IO uint32_t TDCR;
} FDCAN_Config_TypeDef;

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t DIER;
    __IO uint32_t SR;
    __IO uint32_t EGR;
    __IO uint32_t CCMR1;
    uint32_t      RESERVED0;
    __IO uint32_t CCER;
    __IO uint32_t CNT;
    __IO uint32_t PSC;
    __IO uint32_t ARR;
} TIM_TypeDef;

/* ========================================================================= */
/*  Peripheral memory map                                                    */
/* ========================================================================= */

#define FLASH_BASE          0x08000000UL
#define FLASH_R_BASE        0x40022000UL

#define SRAM_BASE           0x20000000UL
#define PERIPH_BASE         0x40000000UL

#define AHBPERIPH_BASE      (PERIPH_BASE + 0x20000UL)
#define APB1PERIPH_BASE     PERIPH_BASE
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x10000UL)

#define GPIOA_BASE          (AHBPERIPH_BASE + 0x0000UL)
#define GPIOB_BASE          (AHBPERIPH_BASE + 0x0400UL)
#define GPIOC_BASE          (AHBPERIPH_BASE + 0x0800UL)
#define GPIOD_BASE          (AHBPERIPH_BASE + 0x0C00UL)
#define GPIOF_BASE          (AHBPERIPH_BASE + 0x1400UL)

#define DMA1_BASE           (AHBPERIPH_BASE - 0x8000UL)
#define DMA1_Channel1_BASE  (DMA1_BASE + 0x0008UL)
#define DMA1_Channel2_BASE  (DMA1_BASE + 0x001CUL)
#define DMA1_Channel3_BASE  (DMA1_BASE + 0x0030UL)
#define DMA1_Channel4_BASE  (DMA1_BASE + 0x0044UL)
#define DMA1_Channel5_BASE  (DMA1_BASE + 0x0058UL)
#define DMA1_Channel6_BASE  (DMA1_BASE + 0x006CUL)
#define DMA1_Channel7_BASE  (DMA1_BASE + 0x0080UL)

#define DMAMUX1_BASE        (AHBPERIPH_BASE - 0x7800UL)
#define DMAMUX1_Channel0_BASE  (DMAMUX1_BASE + 0x0000UL)
#define DMAMUX1_Channel1_BASE  (DMAMUX1_BASE + 0x0004UL)
#define DMAMUX1_Channel2_BASE  (DMAMUX1_BASE + 0x0008UL)
#define DMAMUX1_Channel3_BASE  (DMAMUX1_BASE + 0x000CUL)
#define DMAMUX1_Channel4_BASE  (DMAMUX1_BASE + 0x0010UL)
#define DMAMUX1_Channel5_BASE  (DMAMUX1_BASE + 0x0014UL)
#define DMAMUX1_Channel6_BASE  (DMAMUX1_BASE + 0x0018UL)

#define RCC_BASE            (AHBPERIPH_BASE - 0x1000UL)

#define USART1_BASE         (APB2PERIPH_BASE + 0x3800UL)
#define USART2_BASE         (APB1PERIPH_BASE + 0x4400UL)

#define SPI1_BASE           (APB2PERIPH_BASE + 0x3000UL)
#define SPI2_BASE           (APB1PERIPH_BASE + 0x3800UL)

#define TIM6_BASE           (APB1PERIPH_BASE + 0x1000UL)

#define FDCAN1_BASE         (APB1PERIPH_BASE + 0x8400UL)
#define FDCAN2_BASE         (APB1PERIPH_BASE + 0x8800UL)

/* ========================================================================= */
/*  Peripheral declarations                                                  */
/* ========================================================================= */

#define GPIOA               ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOF               ((GPIO_TypeDef *)GPIOF_BASE)

#define FLASH               ((FLASH_TypeDef *)FLASH_R_BASE)

#define DMA1                ((DMA_TypeDef *)DMA1_BASE)
#define DMA1_Channel1       ((DMA_Channel_TypeDef *)DMA1_Channel1_BASE)
#define DMA1_Channel2       ((DMA_Channel_TypeDef *)DMA1_Channel2_BASE)
#define DMA1_Channel3       ((DMA_Channel_TypeDef *)DMA1_Channel3_BASE)
#define DMA1_Channel4       ((DMA_Channel_TypeDef *)DMA1_Channel4_BASE)
#define DMA1_Channel5       ((DMA_Channel_TypeDef *)DMA1_Channel5_BASE)
#define DMA1_Channel6       ((DMA_Channel_TypeDef *)DMA1_Channel6_BASE)
#define DMA1_Channel7       ((DMA_Channel_TypeDef *)DMA1_Channel7_BASE)

#define DMAMUX1_Channel0    ((DMAMUX_Channel_TypeDef *)DMAMUX1_Channel0_BASE)
#define DMAMUX1_Channel1    ((DMAMUX_Channel_TypeDef *)DMAMUX1_Channel1_BASE)
#define DMAMUX1_Channel2    ((DMAMUX_Channel_TypeDef *)DMAMUX1_Channel2_BASE)
#define DMAMUX1_Channel3    ((DMAMUX_Channel_TypeDef *)DMAMUX1_Channel3_BASE)
#define DMAMUX1_Channel4    ((DMAMUX_Channel_TypeDef *)DMAMUX1_Channel4_BASE)
#define DMAMUX1_Channel5    ((DMAMUX_Channel_TypeDef *)DMAMUX1_Channel5_BASE)
#define DMAMUX1_Channel6    ((DMAMUX_Channel_TypeDef *)DMAMUX1_Channel6_BASE)

#define RCC                 ((RCC_TypeDef *)RCC_BASE)

#define USART1              ((USART_TypeDef *)USART1_BASE)
#define USART2              ((USART_TypeDef *)USART2_BASE)

#define SPI1                ((SPI_TypeDef *)SPI1_BASE)
#define SPI2                ((SPI_TypeDef *)SPI2_BASE)

#define TIM6                ((TIM_TypeDef *)TIM6_BASE)

#define FDCAN1              ((FDCAN_TypeDef *)FDCAN1_BASE)
#define FDCAN2              ((FDCAN_TypeDef *)FDCAN2_BASE)

/* ========================================================================= */
/*  Interrupt numbers (from ARM CMSIS)                                       */
/* ========================================================================= */

typedef enum {
    NonMaskableInt_IRQn         = -14,
    HardFault_IRQn              = -13,
    SVCall_IRQn                 = -5,
    PendSV_IRQn                 = -2,
    SysTick_IRQn                = -1,

    WWDG_IRQn                   = 0,
    PVD_IRQn                    = 1,
    RTC_TAMP_IRQn               = 2,
    FLASH_IRQn                  = 3,
    RCC_IRQn                    = 4,
    EXTI0_1_IRQn                = 5,
    EXTI2_3_IRQn                = 6,
    EXTI4_15_IRQn               = 7,
    USB_UCPD1_2_IRQn            = 8,
    DMA1_Channel1_IRQn          = 9,
    DMA1_Channel2_3_IRQn        = 10,
    DMA1_Channel4_5_6_7_IRQn    = 11,
    ADC1_COMP_IRQn              = 12,
    TIM1_BRK_UP_TRG_COM_IRQn    = 13,
    TIM1_CC_IRQn                = 14,
    TIM2_IRQn                   = 15,
    TIM3_TIM4_IRQn              = 16,
    TIM6_DAC_LPTIM1_IRQn        = 17,
    TIM7_LPTIM2_IRQn            = 18,
    TIM14_IRQn                  = 19,
    TIM15_IRQn                  = 20,
    TIM16_IRQn                  = 21,
    TIM17_IRQn                  = 22,
    I2C1_IRQn                   = 23,
    I2C2_3_IRQn                 = 24,
    SPI1_IRQn                   = 25,
    SPI2_IRQn                   = 26,
    USART1_IRQn                 = 27,
    USART2_IRQn                 = 28,
    USART3_4_5_6_7_8_IRQn       = 29,
    CEC_IRQn                    = 30,
    FDCAN1_IT0_IRQn             = 31,
    FDCAN1_IT1_IRQn             = 32,
    FDCAN2_IT0_IRQn             = 33,
    FDCAN2_IT1_IRQn             = 34,
} IRQn_Type;

/* ========================================================================= */
/*  FLASH register bits                                                       */
/* ========================================================================= */

#define FLASH_ACR_LATENCY_Pos       0U
#define FLASH_ACR_LATENCY_Msk       (0x7UL << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_LATENCY_2WS       (0x2UL << FLASH_ACR_LATENCY_Pos)

/* ========================================================================= */
/*  RCC register bits                                                         */
/* ========================================================================= */

#define RCC_CR_HSION            (1UL << 0)
#define RCC_CR_HSIRDY           (1UL << 2)
#define RCC_CR_PLLON            (1UL << 24)
#define RCC_CR_PLLRDY           (1UL << 25)

#define RCC_CFGR_SW_Pos         0U
#define RCC_CFGR_SW_Msk         (0x3UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_HSI         (0x0UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_PLL         (0x2UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos        2U
#define RCC_CFGR_SWS_Msk        (0x3UL << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_PLL        (0x2UL << RCC_CFGR_SWS_Pos)
#define RCC_CR_PLLRDY           (1UL << 25)

#define RCC_CFGR_SW_Pos         0U
#define RCC_CFGR_SW_Msk         (0x3UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos        2U
#define RCC_CFGR_SWS_Msk        (0x3UL << RCC_CFGR_SWS_Pos)

#define RCC_PLLCFGR_PLLN_Pos    8U
#define RCC_PLLCFGR_PLLSRC_Pos  0U
#define RCC_PLLCFGR_PLLSRC_Msk  (0x3UL << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLSRC_HSI  (0x2UL << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLM_Pos    4U
#define RCC_PLLCFGR_PLLM_Msk    (0x7UL << RCC_PLLCFGR_PLLM_Pos)
#define RCC_PLLCFGR_PLLREN      (1UL << 28)
#define RCC_PLLCFGR_PLLN_Msk    (0x7FUL << RCC_PLLCFGR_PLLN_Pos)
#define RCC_PLLCFGR_PLLR_Pos    29U
#define RCC_PLLCFGR_PLLR_Msk    (0x7UL << RCC_PLLCFGR_PLLR_Pos)

#define RCC_IOPENR_GPIOAEN      (1UL << 0)
#define RCC_IOPENR_GPIOBEN      (1UL << 1)
#define RCC_IOPENR_GPIOCEN      (1UL << 2)
#define RCC_IOPENR_GPIODEN      (1UL << 3)
#define RCC_IOPENR_GPIOFEN      (1UL << 5)

#define RCC_AHBENR_DMA1EN       (1UL << 0)

#define RCC_APBENR1_USART2EN    (1UL << 17)
#define RCC_APBENR1_SPI2EN      (1UL << 14)
#define RCC_APBENR1_TIM6EN      (1UL << 4)
#define RCC_APBENR1_FDCAN1EN    (1UL << 12)
#define RCC_APBENR1_FDCAN2EN    (1UL << 13)

#define RCC_APBENR2_USART1EN    (1UL << 14)
#define RCC_APBENR2_SPI1EN      (1UL << 12)

/* ========================================================================= */
/*  SPI register bits                                                         */
/* ========================================================================= */

#define SPI_CR1_CPHA            (1UL << 0)
#define SPI_CR1_CPOL            (1UL << 1)
#define SPI_CR1_MSTR            (1UL << 2)
#define SPI_CR1_BR_Pos          3U
#define SPI_CR1_BR_Msk          (0x7UL << SPI_CR1_BR_Pos)
#define SPI_CR1_SPE             (1UL << 6)
#define SPI_CR1_SSM             (1UL << 9)
#define SPI_CR1_SSI             (1UL << 10)

#define SPI_CR2_DS_Pos          8U
#define SPI_CR2_DS_Msk          (0xFUL << SPI_CR2_DS_Pos)
#define SPI_CR2_FRXTH           (1UL << 12)
#define SPI_CR2_RXDMAEN         (1UL << 0)
#define SPI_CR2_TXDMAEN         (1UL << 1)
#define SPI_CR2_RXNEIE          (1UL << 6)
#define SPI_CR2_TXEIE           (1UL << 7)

#define SPI_SR_RXNE             (1UL << 0)
#define SPI_SR_TXE              (1UL << 1)
#define SPI_SR_BSY              (1UL << 7)

/* ========================================================================= */
/*  USART register bits                                                       */
/* ========================================================================= */

#define USART_CR1_UE            (1UL << 0)
#define USART_CR1_RE            (1UL << 2)
#define USART_CR1_TE            (1UL << 3)
#define USART_CR1_M0            (1UL << 12)
#define USART_CR1_OVER8         (1UL << 15)

#define USART_ISR_TXE_TXFNF     (1UL << 7)
#define USART_ISR_RXNE_RXFNE    (1UL << 5)

/* ========================================================================= */
/*  DMA register bits                                                         */
/* ========================================================================= */

#define DMA_CCR_EN              (1UL << 0)
#define DMA_CCR_TCIE            (1UL << 1)
#define DMA_CCR_TEIE            (1UL << 3)
#define DMA_CCR_DIR             (1UL << 4)
#define DMA_CCR_MINC            (1UL << 7)
#define DMA_CCR_PINC            (1UL << 6)
#define DMA_CCR_PSIZE_8BIT      (0x0UL << 8)
#define DMA_CCR_MSIZE_8BIT      (0x0UL << 10)

#define DMA_ISR_TCIF3           (1UL << 9)
#define DMA_ISR_TCIF4           (1UL << 13)
#define DMA_IFCR_CGIF3          (1UL << 9)
#define DMA_IFCR_CGIF4          (1UL << 13)

#define DMAMUX_CCR_DMAREQ_ID_Pos 0U
#define DMAMUX_CCR_DMAREQ_ID_Msk (0x7FUL << DMAMUX_CCR_DMAREQ_ID_Pos)

#define DMAMUX_REQ_SPI1_RX      12
#define DMAMUX_REQ_SPI1_TX      13
#define DMAMUX_REQ_SPI2_RX      14
#define DMAMUX_REQ_SPI2_TX      15

/* ========================================================================= */
/*  Timer register bits                                                       */
/* ========================================================================= */

#define TIM_CR_CEN              (1UL << 0)
#define TIM_DIER_UIE            (1UL << 0)
#define TIM_SR_UIF              (1UL << 0)
#define TIM_EGR_UG              (1UL << 0)

/* ========================================================================= */
/*  FDCAN register bits                                                      */
/* ========================================================================= */

#define FDCAN_CCCR_INIT         (1UL << 0)
#define FDCAN_CCCR_CCE          (1UL << 1)
#define FDCAN_NBTP_NBRP_Pos     0U
#define FDCAN_NBTP_NTSEG1_Pos   8U
#define FDCAN_NBTP_NTSEG2_Pos   16U
#define FDCAN_NBTP_NSJW_Pos     24U
#define FDCAN_RXGPC_FLSA_Pos    0U
#define FDCAN_RXGPC_FSE_Pos     8U
#define FDCAN_TXFQS_TFQF        (1UL << 21)
#define FDCAN_PSR_EP            (1UL << 5)
#define FDCAN_PSR_BO            (1UL << 6)
#define FDCAN_IR_RF0N           (1UL << 31)

/* ========================================================================= */
/*  External declarations                                                    */
/* ========================================================================= */

#include "core_cm0plus.h"

extern uint32_t SystemCoreClock;
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
extern void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* STM32G0B1XX_H */
