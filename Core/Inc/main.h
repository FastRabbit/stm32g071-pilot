#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx.h"

/* =========================================================== */
/*  Board configuration (NUCLEO-G071RB)                         */
/* =========================================================== */

/* Green user LED  — LD4 on PA5 */
#define LED_GPIO_PORT       GPIOA
#define LED_GPIO_PIN        5U
#define LED_GPIO_CLK_EN()   SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN)

/* USART1: PA9 TX / PA10 RX  (AF1) */
#define USART1_TX_PORT      GPIOA
#define USART1_TX_PIN       9U
#define USART1_RX_PORT      GPIOA
#define USART1_RX_PIN       10U

/* USART2: PA2 TX / PA3 RX  (AF1)  — printf target */
#define USART2_TX_PORT      GPIOA
#define USART2_TX_PIN       2U
#define USART2_RX_PORT      GPIOA
#define USART2_RX_PIN       3U

/* SPI1: PA5 SCK / PA6 MISO / PA7 MOSI (AF0) */
#define SPI1_SCK_PORT       GPIOA
#define SPI1_SCK_PIN        5U
#define SPI1_MISO_PORT      GPIOA
#define SPI1_MISO_PIN       6U
#define SPI1_MOSI_PORT      GPIOA
#define SPI1_MOSI_PIN       7U

/* Default serial baud rate */
#define UART_BAUD_RATE      115200U

/* System clock — 64 MHz using HSI16 x PLL */
#define SYSTEM_CORE_CLOCK_HZ    64000000UL

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
