/**
 * @file uart.c
 * @brief LL-style USART1 and USART2 driver for STM32G071 @ 64 MHz.
 *
 * USART1: PA9  (TX, AF1) / PA10 (RX, AF1)   — general purpose
 * USART2: PA2  (TX, AF1) / PA3  (RX, AF1)   — printf target
 *
 * Baud rate: 115200
 * BRR = PCLK / baud = 64 000 000 / 115 200 = 556 (rounds to 115 107 baud, <0.1% error)
 */
#include "uart.h"
#include "main.h"
#include <string.h>

/* =========================================================== */
/*  Internal helpers                                             */
/* =========================================================== */

#define UART_BRR_VALUE   556U   /* 64 MHz / 115200 */

/* Configure one GPIO pin as AF mode with the given AF index (0-7 in AFRL) */
static void gpio_set_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
    /* MODER: set bits to 0b10 (alternate function) */
    MODIFY_REG(port->MODER,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));

    /* OTYPER: push-pull (0) */
    CLEAR_BIT(port->OTYPER, (1UL << pin));

    /* OSPEEDR: high speed (0b10) */
    MODIFY_REG(port->OSPEEDR,
               (0x3UL << (pin * 2U)),
               (0x2UL << (pin * 2U)));

    /* PUPDR: no pull (0b00) */
    CLEAR_BIT(port->PUPDR, (0x3UL << (pin * 2U)));

    /* AFR: pins 0-7 use AFR[0] (AFRL), pins 8-15 use AFR[1] (AFRH) */
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

/* Common USART init after clocks and GPIOs are ready */
static void usart_configure(USART_TypeDef *usart)
{
    /* Disable before configuration */
    CLEAR_BIT(usart->CR1, USART_CR1_UE);

    /* 8-bit word, oversampling-16, 1 stop bit (all reset defaults) */
    CLEAR_BIT(usart->CR1, USART_CR1_M0 | USART_CR1_OVER8);
    CLEAR_REG(usart->CR2);  /* 1 stop bit */
    CLEAR_REG(usart->CR3);  /* no flow control */

    /* Baud rate */
    WRITE_REG(usart->BRR, UART_BRR_VALUE);

    /* Enable TX, RX and USART */
    SET_BIT(usart->CR1, USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
}

/* =========================================================== */
/*  USART1 — PA9 TX / PA10 RX  (AF1)                           */
/* =========================================================== */
void UART1_Init(void)
{
    /* GPIO-A clock */
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);

    /* PA9 TX, PA10 RX — AF1 */
    gpio_set_af(GPIOA, 9U,  1U);
    gpio_set_af(GPIOA, 10U, 1U);

    /* USART1 peripheral clock (APB2) */
    SET_BIT(RCC->APBENR2, RCC_APBENR2_USART1EN);

    usart_configure(USART1);
}

/* =========================================================== */
/*  USART2 — PA2 TX / PA3 RX  (AF1)  — printf target           */
/* =========================================================== */
void UART2_Init(void)
{
    /* GPIO-A clock */
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);

    /* PA2 TX, PA3 RX — AF1 */
    gpio_set_af(GPIOA, 2U, 1U);
    gpio_set_af(GPIOA, 3U, 1U);

    /* USART2 peripheral clock (APB1) */
    SET_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN);

    usart_configure(USART2);
}

/* =========================================================== */
/*  TX — blocking on TXE flag                                   */
/* =========================================================== */
void UART1_SendByte(uint8_t byte)
{
    while (!READ_BIT(USART1->ISR, USART_ISR_TXE_TXFNF)) {}
    WRITE_REG(USART1->TDR, byte);
}

void UART2_SendByte(uint8_t byte)
{
    while (!READ_BIT(USART2->ISR, USART_ISR_TXE_TXFNF)) {}
    WRITE_REG(USART2->TDR, byte);
}

void UART1_SendString(const char *str)
{
    while (*str) {
        UART1_SendByte((uint8_t)*str++);
    }
}

void UART2_SendString(const char *str)
{
    while (*str) {
        UART2_SendByte((uint8_t)*str++);
    }
}

void UART1_SendBuffer(const uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        UART1_SendByte(buf[i]);
    }
}

void UART2_SendBuffer(const uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        UART2_SendByte(buf[i]);
    }
}

/* =========================================================== */
/*  RX — non-blocking                                           */
/* =========================================================== */
int UART1_ReceiveByte(uint8_t *byte)
{
    if (READ_BIT(USART1->ISR, USART_ISR_RXNE_RXFNE)) {
        *byte = (uint8_t)READ_REG(USART1->RDR);
        return 1;
    }
    return 0;
}

int UART2_ReceiveByte(uint8_t *byte)
{
    if (READ_BIT(USART2->ISR, USART_ISR_RXNE_RXFNE)) {
        *byte = (uint8_t)READ_REG(USART2->RDR);
        return 1;
    }
    return 0;
}
