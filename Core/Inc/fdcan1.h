#ifndef FDCAN1_H
#define FDCAN1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx.h"
#include <stdint.h>

/* =========================================================== */
/*  FDCAN1 — Interrupt-driven CAN FD                          */
/*  Pins: PB8 RX / PB9 TX (AF3)                              */
/*  Bitrate: 500 kbps (nominal), 1 Mbps (data)                */
/* =========================================================== */

typedef struct {
    uint32_t id;        /* CAN ID (11-bit or 29-bit) */
    uint8_t  dlc;       /* Data length (0-8 bytes) */
    uint8_t  data[8];   /* Payload */
    uint32_t flags;     /* Extended ID, FDF, etc. */
} FDCAN_Message_t;

typedef void (*FDCAN_Callback_t)(const FDCAN_Message_t *msg);

/**
 * @brief Initialize FDCAN1 as CAN 2.0 (500 kbps nominal).
 *        Enables FDCAN1 IRQ in NVIC.
 */
void FDCAN1_Init(void);

/**
 * @brief Transmit a CAN message.
 * @param msg Pointer to message structure.
 * @return 0 on success, -1 if TX FIFO full.
 */
int FDCAN1_SendMessage(const FDCAN_Message_t *msg);

/**
 * @brief Register callback for message reception.
 * @param cb Callback function (called from ISR context).
 */
void FDCAN1_RegisterCallback(FDCAN_Callback_t cb);

/**
 * @brief Check if FDCAN1 is ready (not in error state).
 * @return 1 if ready, 0 otherwise.
 */
int FDCAN1_IsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* FDCAN1_H */
