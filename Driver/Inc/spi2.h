#ifndef SPI2_H
#define SPI2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "spi1.h"   /* SPI_Callback_t */
#include <stdint.h>

/* =========================================================== */
/*  SPI2 — DMA-driven                                           */
/*  Pins: PB13 SCK / PB14 MISO / PB15 MOSI (AF0)              */
/*  DMA1_Channel3 ← SPI2_RX  (DMAMUX req 14)                  */
/*  DMA1_Channel4 → SPI2_TX  (DMAMUX req 15)                   */
/*  Transfer-complete fires on Ch3 (RX done = full xfer done). */
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

#endif /* SPI2_H */
