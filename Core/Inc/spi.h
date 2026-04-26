#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx.h"
#include <stdint.h>

/* =========================================================== */
/*  Callback type                                               */
/* =========================================================== */
typedef void (*SPI_Callback_t)(void);

/* =========================================================== */
/*  SPI1 — Interrupt-driven (PB3 SCK / PB4 MISO / PB5 MOSI)   */
/*                                                               */
/*  NOTE: PA5 is shared with the NUCLEO LD4 LED.  SPI1 uses    */
/*        PB3/4/5 (AF0) instead to avoid that conflict.        */
/*                                                               */
/*  Chip-select is NOT managed by the driver — assert/deassert  */
/*  your CS line in application code before and after calling   */
/*  SPI1_TransmitReceive_IT().                                  */
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

/* =========================================================== */
/*  SPI2 — DMA-driven (PB13 SCK / PB14 MISO / PB15 MOSI)      */
/*                                                               */
/*  DMA channel assignment:                                     */
/*    DMA1_Channel3 ← SPI2_RX  (DMAMUX req 14)                 */
/*    DMA1_Channel4 → SPI2_TX  (DMAMUX req 15)                 */
/*  Transfer-complete fires on Ch3 (RX done = full transfer     */
/*  done in full-duplex).                                       */
/*                                                               */
/*  Chip-select is NOT managed by the driver.                   */
/* =========================================================== */

/**
 * @brief Initialise SPI2 as master, mode 0, 8-bit, fPCLK/8.
 *        Enables DMA1, configures DMAMUX, enables DMA_Ch3 IRQ in NVIC.
 */
void SPI2_Init(void);

/**
 * @brief Start a non-blocking full-duplex SPI2 transfer via DMA.
 *
 * @param tx_buf  Bytes to transmit. Pass NULL to send 0xFF dummy bytes.
 * @param rx_buf  Buffer for received bytes. Pass NULL to discard.
 * @param len     Number of bytes to exchange (must be > 0).
 * @param cb      Called from ISR context when transfer completes. May be NULL.
 * @return 0 on success, -1 if a transfer is already in progress.
 */
int SPI2_TransmitReceive_DMA(const uint8_t *tx_buf, uint8_t *rx_buf,
                              uint16_t len, SPI_Callback_t cb);

/** @brief Returns 1 if SPI2 DMA transfer is in progress, 0 otherwise. */
int SPI2_IsBusy(void);

#ifdef __cplusplus
}
#endif

#endif /* SPI_H */
