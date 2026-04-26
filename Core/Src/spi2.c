/**
 * @file spi2.c
 * @brief SPI2 DMA-driven driver for STM32G071 @ 64 MHz.
 *
 * Pins: PB13 SCK / PB14 MISO / PB15 MOSI (AF0)
 *
 * Master, mode 0 (CPOL=0, CPHA=0), 8-bit, fPCLK/8 = 8 MHz, software NSS.
 *
 * DMA flow:
 *   DMA1_Channel3 ← SPI2_RX  (DMAMUX req 14, periph→mem)
 *   DMA1_Channel4 → SPI2_TX  (DMAMUX req 15, mem→periph)
 *   Only RX channel TC fires the ISR. When last byte is received, the TX
 *   side is guaranteed done in full-duplex, so the callback is safe to call.
 *
 * Chip-select is NOT managed by the driver — assert/deassert your CS GPIO
 * in application code before and after calling SPI2_TransmitReceive_DMA().
 */
#include "spi2.h"
#include <stddef.h>

/* BR[2:0] = 0b010 → fPCLK/8 = 8 MHz at 64 MHz system clock */
#define SPI_BR_DIV   (0x2UL << SPI_CR1_BR_Pos)
/* 8-bit DS[3:0] = 0b0111 */
#define SPI_DS_8BIT  (0x7UL << SPI_CR2_DS_Pos)

static void gpio_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
    MODIFY_REG(port->MODER,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));
    CLEAR_BIT(port->OTYPER, (1UL << pin));
    MODIFY_REG(port->OSPEEDR,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));
    CLEAR_BIT(port->PUPDR, (0x3UL << (pin * 2U)));
    if (pin < 8U) {
        MODIFY_REG(port->AFR[0],
                   (0xFUL << (pin * 4U)),
                   (af    << (pin * 4U)));
    } else {
        uint32_t p = pin - 8U;
        MODIFY_REG(port->AFR[1],
                   (0xFUL << (p * 4U)),
                   (af    << (p * 4U)));
    }
}

typedef struct {
    SPI_Callback_t   cb;
    volatile uint8_t busy;
} SPI2_Handle_t;

static SPI2_Handle_t s2;
static uint8_t       s_dma_rx_dummy;
static const uint8_t s_dma_tx_dummy = 0xFFU;

void SPI2_Init(void)
{
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);

    gpio_af(GPIOB, 13U, 0U);  /* SCK  */
    gpio_af(GPIOB, 14U, 0U);  /* MISO */
    gpio_af(GPIOB, 15U, 0U);  /* MOSI */

    SET_BIT(RCC->APBENR1, RCC_APBENR1_SPI2EN);

    WRITE_REG(SPI2->CR1, SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_BR_DIV);
    WRITE_REG(SPI2->CR2, SPI_DS_8BIT | SPI_CR2_FRXTH);

    /* Enable DMA1 clock */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);

    /* DMAMUX: Ch2 (DMA1_Ch3) = SPI2_RX, Ch3 (DMA1_Ch4) = SPI2_TX */
    MODIFY_REG(DMAMUX1_Channel2->CCR,
               DMAMUX_CCR_DMAREQ_ID_Msk,
               DMAMUX_REQ_SPI2_RX << DMAMUX_CCR_DMAREQ_ID_Pos);
    MODIFY_REG(DMAMUX1_Channel3->CCR,
               DMAMUX_CCR_DMAREQ_ID_Msk,
               DMAMUX_REQ_SPI2_TX << DMAMUX_CCR_DMAREQ_ID_Pos);

    /* Fixed peripheral addresses */
    WRITE_REG(DMA1_Channel3->CPAR, (uint32_t)&SPI2->DR);
    WRITE_REG(DMA1_Channel4->CPAR, (uint32_t)&SPI2->DR);

    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

int SPI2_TransmitReceive_DMA(const uint8_t *tx_buf, uint8_t *rx_buf,
                              uint16_t len, SPI_Callback_t cb)
{
    if (s2.busy || len == 0U) {
        return -1;
    }

    s2.busy = 1U;
    s2.cb   = cb;

    /* Clear pending DMA flags for Ch3 and Ch4 */
    SET_BIT(DMA1->IFCR, DMA_IFCR_CGIF3 | DMA_IFCR_CGIF4);

    /* Configure RX channel (DMA1_Ch3, periph→mem) */
    WRITE_REG(DMA1_Channel3->CNDTR, len);
    if (rx_buf != NULL) {
        WRITE_REG(DMA1_Channel3->CMAR, (uint32_t)rx_buf);
        WRITE_REG(DMA1_Channel3->CCR,
                  DMA_CCR_TCIE       |
                  DMA_CCR_MINC       |
                  DMA_CCR_PSIZE_8BIT |
                  DMA_CCR_MSIZE_8BIT |
                  DMA_CCR_EN);
    } else {
        WRITE_REG(DMA1_Channel3->CMAR, (uint32_t)&s_dma_rx_dummy);
        WRITE_REG(DMA1_Channel3->CCR,
                  DMA_CCR_TCIE       |
                  DMA_CCR_PSIZE_8BIT |
                  DMA_CCR_MSIZE_8BIT |
                  DMA_CCR_EN);
    }

    /* Configure TX channel (DMA1_Ch4, mem→periph) */
    WRITE_REG(DMA1_Channel4->CNDTR, len);
    if (tx_buf != NULL) {
        WRITE_REG(DMA1_Channel4->CMAR, (uint32_t)tx_buf);
        WRITE_REG(DMA1_Channel4->CCR,
                  DMA_CCR_DIR        |
                  DMA_CCR_MINC       |
                  DMA_CCR_PSIZE_8BIT |
                  DMA_CCR_MSIZE_8BIT |
                  DMA_CCR_EN);
    } else {
        WRITE_REG(DMA1_Channel4->CMAR, (uint32_t)&s_dma_tx_dummy);
        WRITE_REG(DMA1_Channel4->CCR,
                  DMA_CCR_DIR        |
                  DMA_CCR_PSIZE_8BIT |
                  DMA_CCR_MSIZE_8BIT |
                  DMA_CCR_EN);
    }

    /* Enable SPI DMA requests and SPI */
    SET_BIT(SPI2->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
    SET_BIT(SPI2->CR1, SPI_CR1_SPE);

    return 0;
}

int SPI2_IsBusy(void)
{
    return (int)s2.busy;
}

/* Fires when DMA1_Channel3 (SPI2 RX) transfer completes */
void DMA1_Channel2_3_IRQHandler(void)
{
    if (READ_BIT(DMA1->ISR, DMA_ISR_TCIF3)) {
        CLEAR_BIT(DMA1_Channel3->CCR, DMA_CCR_EN);
        CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_EN);
        SET_BIT(DMA1->IFCR, DMA_IFCR_CGIF3 | DMA_IFCR_CGIF4);

        while (READ_BIT(SPI2->SR, SPI_SR_BSY)) {}
        CLEAR_BIT(SPI2->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
        CLEAR_BIT(SPI2->CR1, SPI_CR1_SPE);

        s2.busy = 0U;
        if (s2.cb != NULL) {
            s2.cb();
        }
    }
}
