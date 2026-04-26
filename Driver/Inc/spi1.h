#ifndef SPI1_H
#define SPI1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx.h"
#include <stdint.h>

/* =========================================================== */
/*  Shared callback type (also used by spi2.h)                  */
/* =========================================================== */
typedef void (*SPI_Callback_t)(void);

/* =========================================================== */
/*  SPI1 — Interrupt-driven                                     */
/*  Pins: PB3 SCK / PB4 MISO / PB5 MOSI (AF0)                 */
/*  NOTE: PA5 is the NUCLEO LED — PB3/4/5 avoids that conflict. */
/*  Chip-select is NOT managed by the driver.                   */
/* =========================================================== */

/**
 * @brief Initialise SPI1 as master, mode 0, 8-bit, fPCLK/8 (8 MHz @ 64 MHz).
 *        Enables the SPI1 IRQ in NVIC.
 */
void SPI1_Init(void);

/**
 * @brief Start a non-blocking full-duplex SPI1 transfer.
 *
 * @param tx_buf  Bytes to transmit. Pass NULL to send 0xFF dummy bytes.
 * @param rx_buf  Buffer for received bytes. Pass NULL to discard.
 * @param len     Number of bytes to exchange (must be > 0).
 * @param cb      Called from ISR context when transfer completes. May be NULL.
 * @return 0 on success, -1 if a transfer is already in progress.
 */
int SPI1_TransmitReceive_IT(const uint8_t *tx_buf, uint8_t *rx_buf,
                             uint16_t len, SPI_Callback_t cb);

/** @brief Returns 1 if SPI1 is busy with a transfer, 0 otherwise. */
int SPI1_IsBusy(void);

#ifdef __cplusplus
}
#endif

#endif /* SPI1_H */
