#include "fdcan_codec.h"
#include <string.h>

static const uint8_t k_dlc_to_len[16] = {
    0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U,
    8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U
};

uint8_t FDCAN_Codec_DlcToLength(uint8_t dlc)
{
    if (dlc > 15U) {
        return 64U;
    }
    return k_dlc_to_len[dlc];
}

uint8_t FDCAN_Codec_LengthToDlc(uint8_t length)
{
    if (length <= 8U) {
        return length;
    }
    if (length <= 12U) {
        return 9U;
    }
    if (length <= 16U) {
        return 10U;
    }
    if (length <= 20U) {
        return 11U;
    }
    if (length <= 24U) {
        return 12U;
    }
    if (length <= 32U) {
        return 13U;
    }
    if (length <= 48U) {
        return 14U;
    }
    return 15U;
}

int FDCAN_Codec_IsClassicLengthValid(uint8_t length)
{
    return length <= 8U;
}

int FDCAN_Codec_IsClassicDlcValid(uint8_t dlc)
{
    return dlc <= 8U;
}

/* =========================================================== */
/*  TX element packing                                         */
/*                                                             */
/*  T0 [31:0]:                                                 */
/*    [28:18] STDID (11-bit CAN ID)                           */
/*    [29]    XTD   (0 = standard, 1 = extended)              */
/*    [30]    RTR   (0 = data frame)                           */
/*    [31]    ESI   (0 = error active)                         */
/*  T1 [31:0]:                                                 */
/*    [3:0]   DLC                                              */
/*    [20]    FDF   (0 = classic CAN)                          */
/*    [23]    EFC   (0 = no TX event)                          */
/*    [31:24] MM    (message marker, 0)                        */
/*  T2/T3: data bytes 0-3 / 4-7 (little-endian words)        */
/* =========================================================== */

void FDCAN_Codec_PackTxElement(uint32_t buf[FDCAN_ELEMENT_WORDS],
                               const FDCAN_Message_t *msg)
{
    /* T0 */
    buf[0] = ((uint32_t)(msg->id & 0x7FFU) << 18U)
           | ((msg->flags & 0x1U) ? (1UL << 29U) : 0UL)   /* XTD */
           | ((msg->flags & 0x2U) ? (1UL << 30U) : 0UL);  /* RTR */

    /* T1 */
    buf[1] = (uint32_t)(msg->dlc & 0xFU);

    /* T2: bytes 0-3 */
    buf[2] = (uint32_t)msg->data[0]
           | ((uint32_t)msg->data[1] << 8U)
           | ((uint32_t)msg->data[2] << 16U)
           | ((uint32_t)msg->data[3] << 24U);

    /* T3: bytes 4-7 */
    buf[3] = (uint32_t)msg->data[4]
           | ((uint32_t)msg->data[5] << 8U)
           | ((uint32_t)msg->data[6] << 16U)
           | ((uint32_t)msg->data[7] << 24U);
}

/* =========================================================== */
/*  RX element unpacking                                       */
/*                                                             */
/*  R0 [31:0]:                                                 */
/*    [28:18] STDID when XTD=0 / [28:0] EXTID when XTD=1     */
/*    [29]    XTD                                              */
/*    [30]    RTR                                              */
/*  R1 [31:0]:                                                 */
/*    [3:0]   DLC                                              */
/*  R2/R3: data bytes 0-3 / 4-7                               */
/* =========================================================== */

void FDCAN_Codec_UnpackRxElement(const uint32_t buf[FDCAN_ELEMENT_WORDS],
                                 FDCAN_Message_t *msg)
{
    uint32_t xtd = (buf[0] >> 29U) & 0x1U;
    uint32_t rtr = (buf[0] >> 30U) & 0x1U;

    msg->flags = (xtd ? 0x1U : 0U) | (rtr ? 0x2U : 0U);

    if (xtd) {
        msg->id = buf[0] & 0x1FFFFFFFU;      /* 29-bit extended ID */
    } else {
        msg->id = (buf[0] >> 18U) & 0x7FFU;  /* 11-bit standard ID */
    }

    msg->dlc = (uint8_t)(buf[1] & 0xFU);

    msg->data[0] = (uint8_t)(buf[2] & 0xFFU);
    msg->data[1] = (uint8_t)((buf[2] >>  8U) & 0xFFU);
    msg->data[2] = (uint8_t)((buf[2] >> 16U) & 0xFFU);
    msg->data[3] = (uint8_t)((buf[2] >> 24U) & 0xFFU);
    msg->data[4] = (uint8_t)(buf[3] & 0xFFU);
    msg->data[5] = (uint8_t)((buf[3] >>  8U) & 0xFFU);
    msg->data[6] = (uint8_t)((buf[3] >> 16U) & 0xFFU);
    msg->data[7] = (uint8_t)((buf[3] >> 24U) & 0xFFU);
}
