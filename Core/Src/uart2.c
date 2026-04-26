/**
 * @file uart2.c
 * @brief LL-style USART2 driver for STM32G071 @ 64 MHz.
 *
 * USART2: PA2 (TX, AF1) / PA3 (RX, AF1) — printf target (_write routes here)
 * Baud rate: 115200  (BRR = 64 000 000 / 115 200 = 556)
 */
#include "uart2.h"
#include "main.h"

#define UART_BRR_VALUE  556U

static void gpio_set_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
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

static void usart_configure(USART_TypeDef *usart)
{
    CLEAR_BIT(usart->CR1, USART_CR1_UE);
    CLEAR_BIT(usart->CR1, USART_CR1_M0 | USART_CR1_OVER8);
    CLEAR_REG(usart->CR2);
    CLEAR_REG(usart->CR3);
    WRITE_REG(usart->BRR, UART_BRR_VALUE);
    SET_BIT(usart->CR1, USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
}

void UART2_Init(void)
{
    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);
    gpio_set_af(GPIOA, 2U, 1U);
    gpio_set_af(GPIOA, 3U, 1U);
    SET_BIT(RCC->APBENR1, RCC_APBENR1_USART2EN);
    usart_configure(USART2);
}

void UART2_SendByte(uint8_t byte)
{
    while (!READ_BIT(USART2->ISR, USART_ISR_TXE_TXFNF)) {}
    WRITE_REG(USART2->TDR, byte);
}

void UART2_SendString(const char *str)
{
    while (*str) {
        UART2_SendByte((uint8_t)*str++);
    }
}

void UART2_SendBuffer(const uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        UART2_SendByte(buf[i]);
    }
}

int UART2_ReceiveByte(uint8_t *byte)
{
    if (READ_BIT(USART2->ISR, USART_ISR_RXNE_RXFNE)) {
        *byte = (uint8_t)READ_REG(USART2->RDR);
        return 1;
    }
    return 0;
}
