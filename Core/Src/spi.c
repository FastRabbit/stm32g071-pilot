/**
 * @file spi.c
 * @brief SPI1 (interrupt-driven) and SPI2 (DMA-driven) for STM32G071 @ 64 MHz.
 *
 * SPI1  pins: PB3 SCK / PB4 MISO / PB5 MOSI  (AF0)
 * SPI2  pins: PB13 SCK / PB14 MISO / PB15 MOSI (AF0)
 *
 * Both configured as SPI master, mode 0 (CPOL=0, CPHA=0),
 * 8-bit data frame, fPCLK/8 baud rate, software NSS.
 *
 * SPI1 IRQ flow (full-duplex):
 *   1. TransmitReceive_IT loads the handle and writes the first byte.
 *   2. RXNE ISR: reads received byte, writes next TX byte (or dummy 0xFF).
 *   3. When all bytes have been received the ISR disables SPI1 interrupts,
 *      clears the busy flag and calls the user callback.
 *
 * SPI2 DMA flow:
 *   DMA1_Ch3 (RX, periph→mem) and DMA1_Ch4 (TX, mem→periph) run in parallel.
 *   Only the RX channel raises a TC interrupt — when RX is complete the full
 *   duplex exchange is guaranteed to be finished.
 *   The ISR disables both DMA channels, disables SPI DMA requests, clears
 *   the busy flag and calls the user callback.
 */
#include "spi.h"
#include <stddef.h>

/* =========================================================== */
/*  SPI configuration constants                                 */
/* =========================================================== */

/*
 * BR[2:0] = 0b010 → fPCLK / 8 = 8 MHz at 64 MHz system clock.
 * Adjust SPI_BR_DIV as needed (0=÷2, 1=÷4, 2=÷8, 3=÷16 …).
 */
#define SPI_BR_DIV   (0x2UL << SPI_CR1_BR_Pos)

/* 8-bit DS[3:0] = 0b0111 */
#define SPI_DS_8BIT  (0x7UL << SPI_CR2_DS_Pos)

/* =========================================================== */
/*  Internal helpers                                            */
/* =========================================================== */

static void gpio_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
    /* MODER = 0b10 (alternate function) */
    MODIFY_REG(port->MODER,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));
    /* OTYPER = push-pull */
    CLEAR_BIT(port->OTYPER, (1UL << pin));
    /* OSPEEDR = high speed */
    MODIFY_REG(port->OSPEEDR,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));
    /* PUPDR = no pull */
    CLEAR_BIT(port->PUPDR, (0x3UL << (pin * 2U)));
    /* AFR (AFRL for pins 0-7, AFRH for 8-15) */
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

/* =========================================================== */
/*  ── SPI1 — IRQ-driven ──────────────────────────────────── */
/* =========================================================== */

typedef struct {
    const uint8_t   *tx;
    uint8_t         *rx;
    uint16_t         len;
    uint16_t         tx_idx;
    uint16_t         rx_idx;
    SPI_Callback_t   cb;
    volatile uint8_t busy;
} SPI1_Handle_t;

static SPI1_Handle_t s1;
static const uint8_t s_tx_dummy = 0xFFU;   /* sent when tx_buf is NULL */

void SPI1_Init(void)
{
    /* Enable GPIOB clock */
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);

    /* PB3 SCK, PB4 MISO, PB5 MOSI — AF0 */
    gpio_af(GPIOB, 3U, 0U);
    gpio_af(GPIOB, 4U, 0U);
    gpio_af(GPIOB, 5U, 0U);

    /* SPI1 peripheral clock (APB2) */
    SET_BIT(RCC->APBENR2, RCC_APBENR2_SPI1EN);

    /* Configure SPI1:
     *   Master, mode 0 (CPOL=0 CPHA=0), software NSS (SSM+SSI),
     *   baud = fPCLK/8, SPI disabled until first transfer.
     */
    WRITE_REG(SPI1->CR1, SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_BR_DIV);
    WRITE_REG(SPI1->CR2, SPI_DS_8BIT | SPI_CR2_FRXTH); /* 8-bit, FIFO quarter-full */

    /* Enable SPI1 IRQ in NVIC at default priority */
    NVIC_EnableIRQ(SPI1_IRQn);
}

int SPI1_TransmitReceive_IT(const uint8_t *tx_buf, uint8_t *rx_buf,
                             uint16_t len, SPI_Callback_t cb)
{
    if (s1.busy || len == 0U) {
        return -1;
    }

    /* Load transfer descriptor */
    s1.tx     = (tx_buf != NULL) ? tx_buf : &s_tx_dummy;
    s1.rx     = rx_buf;
    s1.len    = len;
    s1.tx_idx = 0U;
    s1.rx_idx = 0U;
    s1.cb     = cb;
    s1.busy   = 1U;

    /* Enable SPI */
    SET_BIT(SPI1->CR1, SPI_CR1_SPE);

    /* Enable RXNE interrupt (TXEIE will be managed in the ISR).
     * Write first byte to kick off the transfer — TXE is asserted at SPE. */
    SET_BIT(SPI1->CR2, SPI_CR2_RXNEIE | SPI_CR2_TXEIE);

    return 0;
}

int SPI1_IsBusy(void)
{
    return (int)s1.busy;
}

/* ----------------------------------------------------------
 * SPI1_IRQHandler
 *
 * Called on TXE (TX buffer empty) and RXNE (RX buffer not empty).
 * We drive TX from TXE and collect RX from RXNE.
 * ---------------------------------------------------------- */
void SPI1_IRQHandler(void)
{
    uint32_t sr = READ_REG(SPI1->SR);

    /* ---- TX: write next byte ---- */
    if (READ_BIT(SPI1->CR2, SPI_CR2_TXEIE) && (sr & SPI_SR_TXE)) {
        if (s1.tx_idx < s1.len) {
            /* If tx_buf was NULL the pointer was set to &s_tx_dummy (no MINC) */
            uint8_t byte = (s1.tx == &s_tx_dummy) ? 0xFFU : s1.tx[s1.tx_idx];
            s1.tx_idx++;
            *(volatile uint8_t *)&SPI1->DR = byte;
        } else {
            /* All bytes sent — stop driving TX */
            CLEAR_BIT(SPI1->CR2, SPI_CR2_TXEIE);
        }
    }

    /* ---- RX: collect received byte ---- */
    if (sr & SPI_SR_RXNE) {
        uint8_t byte = *(volatile uint8_t *)&SPI1->DR;
        if (s1.rx != NULL) {
            s1.rx[s1.rx_idx] = byte;
        }
        s1.rx_idx++;

        if (s1.rx_idx >= s1.len) {
            /* Transfer complete */
            CLEAR_BIT(SPI1->CR2, SPI_CR2_RXNEIE | SPI_CR2_TXEIE);

            /* Wait for BSY to clear then disable SPI */
            while (READ_BIT(SPI1->SR, SPI_SR_BSY)) {}
            CLEAR_BIT(SPI1->CR1, SPI_CR1_SPE);

            s1.busy = 0U;
            if (s1.cb != NULL) {
                s1.cb();
            }
        }
    }
}

/* =========================================================== */
/*  ── SPI2 — DMA-driven ─────────────────────────────────── */
/*                                                              */
/*  DMA1_Channel3  ← SPI2 RX  (DMAMUX req 14, periph→mem)    */
/*  DMA1_Channel4  → SPI2 TX  (DMAMUX req 15, mem→periph)    */
/*                                                              */
/*  Only RX channel TC fires the ISR. When the last byte is   */
/*  received, the TX side is guaranteed done.                  */
/* =========================================================== */

typedef struct {
    SPI_Callback_t   cb;
    volatile uint8_t busy;
} SPI2_Handle_t;

static SPI2_Handle_t s2;
static uint8_t       s_dma_rx_dummy;       /* sink when rx_buf is NULL */
static const uint8_t s_dma_tx_dummy = 0xFFU; /* source when tx_buf is NULL */

void SPI2_Init(void)
{
    /* Enable GPIOB clock */
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);

    /* PB13 SCK, PB14 MISO, PB15 MOSI — AF0 */
    gpio_af(GPIOB, 13U, 0U);
    gpio_af(GPIOB, 14U, 0U);
    gpio_af(GPIOB, 15U, 0U);

    /* SPI2 peripheral clock (APB1) */
    SET_BIT(RCC->APBENR1, RCC_APBENR1_SPI2EN);

    /* Configure SPI2 — same settings as SPI1 */
    WRITE_REG(SPI2->CR1, SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_BR_DIV);
    WRITE_REG(SPI2->CR2, SPI_DS_8BIT | SPI_CR2_FRXTH);

    /* ---- DMA setup ---- */

    /* Enable DMA1 clock (AHB) */
    SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);

    /* DMAMUX: map Ch3 (DMA1_Ch3 RX) and Ch4 (DMA1_Ch4 TX) */
    MODIFY_REG(DMAMUX1_Channel2->CCR,
               DMAMUX_CCR_DMAREQ_ID_Msk,
               DMAMUX_REQ_SPI2_RX << DMAMUX_CCR_DMAREQ_ID_Pos);

    MODIFY_REG(DMAMUX1_Channel3->CCR,
               DMAMUX_CCR_DMAREQ_ID_Msk,
               DMAMUX_REQ_SPI2_TX << DMAMUX_CCR_DMAREQ_ID_Pos);

    /* Fixed peripheral address for both channels */
    WRITE_REG(DMA1_Channel3->CPAR, (uint32_t)&SPI2->DR);
    WRITE_REG(DMA1_Channel4->CPAR, (uint32_t)&SPI2->DR);

    /* Enable DMA1_Channel2_3 IRQ in NVIC (Ch3 TC fires here) */
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

    /* Clear any pending DMA flags for Ch3 and Ch4 */
    SET_BIT(DMA1->IFCR, DMA_IFCR_CGIF3 | DMA_IFCR_CGIF4);

    /* ---- Configure RX channel (DMA1_Ch3, periph→mem) ---- */
    WRITE_REG(DMA1_Channel3->CNDTR, len);

    if (rx_buf != NULL) {
        WRITE_REG(DMA1_Channel3->CMAR, (uint32_t)rx_buf);
        WRITE_REG(DMA1_Channel3->CCR,
                  DMA_CCR_TCIE      |   /* TC interrupt when RX done */
                  DMA_CCR_MINC      |   /* increment memory address  */
                  DMA_CCR_PSIZE_8BIT|
                  DMA_CCR_MSIZE_8BIT|
                  DMA_CCR_EN);
    } else {
        /* Discard RX: memory address fixed on dummy byte, no MINC */
        WRITE_REG(DMA1_Channel3->CMAR, (uint32_t)&s_dma_rx_dummy);
        WRITE_REG(DMA1_Channel3->CCR,
                  DMA_CCR_TCIE      |
                  DMA_CCR_PSIZE_8BIT|
                  DMA_CCR_MSIZE_8BIT|
                  DMA_CCR_EN);
    }

    /* ---- Configure TX channel (DMA1_Ch4, mem→periph) ---- */
    WRITE_REG(DMA1_Channel4->CNDTR, len);

    if (tx_buf != NULL) {
        WRITE_REG(DMA1_Channel4->CMAR, (uint32_t)tx_buf);
        WRITE_REG(DMA1_Channel4->CCR,
                  DMA_CCR_DIR       |   /* mem→periph */
                  DMA_CCR_MINC      |
                  DMA_CCR_PSIZE_8BIT|
                  DMA_CCR_MSIZE_8BIT|
                  DMA_CCR_EN);
    } else {
        /* Dummy TX: fixed address on 0xFF byte, no MINC */
        WRITE_REG(DMA1_Channel4->CMAR, (uint32_t)&s_dma_tx_dummy);
        WRITE_REG(DMA1_Channel4->CCR,
                  DMA_CCR_DIR       |
                  DMA_CCR_PSIZE_8BIT|
                  DMA_CCR_MSIZE_8BIT|
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

/* ----------------------------------------------------------
 * DMA1_Channel2_3_IRQHandler
 *
 * Fires when DMA1_Channel3 (SPI2 RX) transfer completes.
 * At this point all bytes have been both sent and received.
 * ---------------------------------------------------------- */
void DMA1_Channel2_3_IRQHandler(void)
{
    if (READ_BIT(DMA1->ISR, DMA_ISR_TCIF3)) {
        /* Disable both DMA channels */
        CLEAR_BIT(DMA1_Channel3->CCR, DMA_CCR_EN);
        CLEAR_BIT(DMA1_Channel4->CCR, DMA_CCR_EN);

        /* Clear all Ch3 and Ch4 flags */
        SET_BIT(DMA1->IFCR, DMA_IFCR_CGIF3 | DMA_IFCR_CGIF4);

        /* Wait for BSY and disable SPI DMA requests + SPI */
        while (READ_BIT(SPI2->SR, SPI_SR_BSY)) {}
        CLEAR_BIT(SPI2->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
        CLEAR_BIT(SPI2->CR1, SPI_CR1_SPE);

        s2.busy = 0U;
        if (s2.cb != NULL) {
            s2.cb();
        }
    }
}
