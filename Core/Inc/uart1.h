#ifndef UART1_H
#define UART1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx.h"
#include <stdint.h>

/**
 * @brief Initialise USART1 (PA9 TX / PA10 RX) at 115200 8N1.
 *        Peripheral and GPIO clocks are enabled inside.
 */
void UART1_Init(void);

/** @brief Transmit a single byte, blocking until TXE. */
void UART1_SendByte(uint8_t byte);

/** @brief Transmit a null-terminated string. */
void UART1_SendString(const char *str);

/** @brief Transmit a buffer of known length. */
void UART1_SendBuffer(const uint8_t *buf, uint32_t len);

/**
 * @brief Non-blocking receive.
 * @return 1 and writes *byte if data is available, 0 otherwise.
 */
int UART1_ReceiveByte(uint8_t *byte);

#ifdef __cplusplus
}
#endif

#endif /* UART1_H */
