#ifndef FDCAN_CODEC_H
#define FDCAN_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* =========================================================== */
/*  CAN message data model (hardware-independent)              */
/* =========================================================== */

typedef struct {
    uint32_t id;        /* CAN ID: 11-bit (std) or 29-bit (ext) */
    uint8_t  dlc;       /* Data length code 0..8 (classic CAN) */
    uint8_t  data[8];   /* Payload bytes */
    uint32_t flags;     /* Bit 0: XTD (extended ID), Bit 1: RTR */
} FDCAN_Message_t;

/* =========================================================== */
/*  DLC / length conversion helpers                            */
/* =========================================================== */

uint8_t FDCAN_Codec_DlcToLength(uint8_t dlc);
uint8_t FDCAN_Codec_LengthToDlc(uint8_t length);
int     FDCAN_Codec_IsClassicLengthValid(uint8_t length);
int     FDCAN_Codec_IsClassicDlcValid(uint8_t dlc);

/* =========================================================== */
/*  TX / RX element packing (hardware-independent)             */
/*                                                             */
/*  Each element is 4 x uint32_t words (classic CAN, 8 bytes): */
/*    [0] T0/R0 : ID + control bits                           */
/*    [1] T1/R1 : DLC + format flags                          */
/*    [2] data bytes 0-3                                       */
/*    [3] data bytes 4-7                                       */
/* =========================================================== */

#define FDCAN_ELEMENT_WORDS  4U

/** Pack FDCAN_Message_t into 4-word TX element (T0/T1/data). */
void FDCAN_Codec_PackTxElement(uint32_t buf[FDCAN_ELEMENT_WORDS],
                               const FDCAN_Message_t *msg);

/** Unpack 4-word RX element (R0/R1/data) into FDCAN_Message_t. */
void FDCAN_Codec_UnpackRxElement(const uint32_t buf[FDCAN_ELEMENT_WORDS],
                                 FDCAN_Message_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* FDCAN_CODEC_H */
