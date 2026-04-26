/**
 * @file spi1.c
 * @brief SPI1 interrupt-driven driver for STM32G071 @ 64 MHz.
 *
 * Pins: PB3 SCK / PB4 MISO / PB5 MOSI (AF0)
 * NOTE: PA5 is the NUCLEO LD4 LED — PB3/4/5 avoids that conflict.
 *
 * Master, mode 0 (CPOL=0, CPHA=0), 8-bit, fPCLK/8 = 8 MHz, software NSS.
 *
 * IRQ flow (full-duplex):
 *   1. TransmitReceive_IT loads the handle and enables TXE+RXNE interrupts.
 *   2. TXE ISR: writes next TX byte (or 0xFF dummy); disables TXEIE when done.
 *   3. RXNE ISR: stores received byte; on last byte disables SPI and calls cb.
 *
 * Chip-select is NOT managed by the driver — assert/deassert your CS GPIO
 * in application code before and after calling SPI1_TransmitReceive_IT().
 */
#include "spi1.h"
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
    const uint8_t   *tx;
    uint8_t         *rx;
    uint16_t         len;
    uint16_t         tx_idx;
    uint16_t         rx_idx;
    SPI_Callback_t   cb;
    volatile uint8_t busy;
} SPI1_Handle_t;

static SPI1_Handle_t s1;
static const uint8_t s_tx_dummy = 0xFFU;

void SPI1_Init(void)
{
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOBEN);

    gpio_af(GPIOB, 3U, 0U);  /* SCK  */
    gpio_af(GPIOB, 4U, 0U);  /* MISO */
    gpio_af(GPIOB, 5U, 0U);  /* MOSI */

    SET_BIT(RCC->APBENR2, RCC_APBENR2_SPI1EN);

    WRITE_REG(SPI1->CR1, SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_BR_DIV);
    WRITE_REG(SPI1->CR2, SPI_DS_8BIT | SPI_CR2_FRXTH);

    NVIC_EnableIRQ(SPI1_IRQn);
}

int SPI1_TransmitReceive_IT(const uint8_t *tx_buf, uint8_t *rx_buf,
                             uint16_t len, SPI_Callback_t cb)
{
    if (s1.busy || len == 0U) {
        return -1;
    }

    s1.tx     = (tx_buf != NULL) ? tx_buf : &s_tx_dummy;
    s1.rx     = rx_buf;
    s1.len    = len;
    s1.tx_idx = 0U;
    s1.rx_idx = 0U;
    s1.cb     = cb;
    s1.busy   = 1U;

    SET_BIT(SPI1->CR1, SPI_CR1_SPE);
    SET_BIT(SPI1->CR2, SPI_CR2_RXNEIE | SPI_CR2_TXEIE);

    return 0;
}

int SPI1_IsBusy(void)
{
    return (int)s1.busy;
}

void SPI1_IRQHandler(void)
{
    uint32_t sr = READ_REG(SPI1->SR);

    /* TX: write next byte when TX buffer empty */
    if (READ_BIT(SPI1->CR2, SPI_CR2_TXEIE) && (sr & SPI_SR_TXE)) {
        if (s1.tx_idx < s1.len) {
            uint8_t byte = (s1.tx == &s_tx_dummy) ? 0xFFU : s1.tx[s1.tx_idx];
            s1.tx_idx++;
            *(volatile uint8_t *)&SPI1->DR = byte;
        } else {
            CLEAR_BIT(SPI1->CR2, SPI_CR2_TXEIE);
        }
    }

    /* RX: collect received byte */
    if (sr & SPI_SR_RXNE) {
        uint8_t byte = *(volatile uint8_t *)&SPI1->DR;
        if (s1.rx != NULL) {
            s1.rx[s1.rx_idx] = byte;
        }
        s1.rx_idx++;

        if (s1.rx_idx >= s1.len) {
            CLEAR_BIT(SPI1->CR2, SPI_CR2_RXNEIE | SPI_CR2_TXEIE);
            while (READ_BIT(SPI1->SR, SPI_SR_BSY)) {}
            CLEAR_BIT(SPI1->CR1, SPI_CR1_SPE);
            s1.busy = 0U;
            if (s1.cb != NULL) {
                s1.cb();
            }
        }
    }
}
