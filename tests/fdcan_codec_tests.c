#include <stdio.h>
#include <string.h>

#include "fdcan_codec.h"

static int expect_eq_u8(unsigned int expected, unsigned int actual, const char *what)
{
    if (expected != actual) {
        fprintf(stderr, "FAIL: %s expected=%u actual=%u\n", what, expected, actual);
        return 1;
    }
    return 0;
}

static int expect_eq_u32(uint32_t expected, uint32_t actual, const char *what)
{
    if (expected != actual) {
        fprintf(stderr, "FAIL: %s expected=0x%08X actual=0x%08X\n",
                what, (unsigned)expected, (unsigned)actual);
        return 1;
    }
    return 0;
}

/* =========================================================== */
/*  DLC / length helpers                                       */
/* =========================================================== */

static int test_dlc_to_length(void)
{
    int fails = 0;
    fails += expect_eq_u8(0U, FDCAN_Codec_DlcToLength(0U), "dlc 0 -> len");
    fails += expect_eq_u8(8U, FDCAN_Codec_DlcToLength(8U), "dlc 8 -> len");
    fails += expect_eq_u8(12U, FDCAN_Codec_DlcToLength(9U), "dlc 9 -> len");
    fails += expect_eq_u8(16U, FDCAN_Codec_DlcToLength(10U), "dlc 10 -> len");
    fails += expect_eq_u8(20U, FDCAN_Codec_DlcToLength(11U), "dlc 11 -> len");
    fails += expect_eq_u8(24U, FDCAN_Codec_DlcToLength(12U), "dlc 12 -> len");
    fails += expect_eq_u8(32U, FDCAN_Codec_DlcToLength(13U), "dlc 13 -> len");
    fails += expect_eq_u8(48U, FDCAN_Codec_DlcToLength(14U), "dlc 14 -> len");
    fails += expect_eq_u8(64U, FDCAN_Codec_DlcToLength(15U), "dlc 15 -> len");
    return fails;
}

static int test_length_to_dlc(void)
{
    int fails = 0;
    fails += expect_eq_u8(0U, FDCAN_Codec_LengthToDlc(0U), "len 0 -> dlc");
    fails += expect_eq_u8(8U, FDCAN_Codec_LengthToDlc(8U), "len 8 -> dlc");
    fails += expect_eq_u8(9U, FDCAN_Codec_LengthToDlc(9U), "len 9 -> dlc");
    fails += expect_eq_u8(9U, FDCAN_Codec_LengthToDlc(12U), "len 12 -> dlc");
    fails += expect_eq_u8(10U, FDCAN_Codec_LengthToDlc(13U), "len 13 -> dlc");
    fails += expect_eq_u8(10U, FDCAN_Codec_LengthToDlc(16U), "len 16 -> dlc");
    fails += expect_eq_u8(15U, FDCAN_Codec_LengthToDlc(63U), "len 63 -> dlc");
    fails += expect_eq_u8(15U, FDCAN_Codec_LengthToDlc(64U), "len 64 -> dlc");
    fails += expect_eq_u8(15U, FDCAN_Codec_LengthToDlc(65U), "len 65 -> dlc");
    return fails;
}

static int test_classic_validation(void)
{
    int fails = 0;
    fails += expect_eq_u8(1U, (unsigned int)FDCAN_Codec_IsClassicDlcValid(0U), "classic dlc 0 valid");
    fails += expect_eq_u8(1U, (unsigned int)FDCAN_Codec_IsClassicDlcValid(8U), "classic dlc 8 valid");
    fails += expect_eq_u8(0U, (unsigned int)FDCAN_Codec_IsClassicDlcValid(9U), "classic dlc 9 invalid");
    fails += expect_eq_u8(1U, (unsigned int)FDCAN_Codec_IsClassicLengthValid(0U), "classic len 0 valid");
    fails += expect_eq_u8(1U, (unsigned int)FDCAN_Codec_IsClassicLengthValid(8U), "classic len 8 valid");
    fails += expect_eq_u8(0U, (unsigned int)FDCAN_Codec_IsClassicLengthValid(9U), "classic len 9 invalid");
    return fails;
}

/* =========================================================== */
/*  TX element packing                                         */
/* =========================================================== */

static int test_pack_standard_id(void)
{
    int fails = 0;
    FDCAN_Message_t msg;
    uint32_t buf[FDCAN_ELEMENT_WORDS];

    memset(&msg, 0, sizeof(msg));
    msg.id    = 0x123U;
    msg.dlc   = 3U;
    msg.data[0] = 0xAAU;
    msg.data[1] = 0xBBU;
    msg.data[2] = 0xCCU;
    msg.flags   = 0U;   /* standard ID, data frame */

    FDCAN_Codec_PackTxElement(buf, &msg);

    /* T0: ID 0x123 shifted to [28:18] = 0x123 << 18 = 0x048C0000 */
    fails += expect_eq_u32(0x048C0000UL, buf[0], "T0 standard ID");
    /* T1: DLC = 3 */
    fails += expect_eq_u32(3U, buf[1], "T1 DLC");
    /* T2: byte0=0xAA, byte1=0xBB, byte2=0xCC, byte3=0x00 */
    fails += expect_eq_u32(0x00CCBBAAUL, buf[2], "T2 data bytes 0-3");
    /* T3: all zeros */
    fails += expect_eq_u32(0U, buf[3], "T3 data bytes 4-7");

    return fails;
}

static int test_pack_rtr_flag(void)
{
    int fails = 0;
    FDCAN_Message_t msg;
    uint32_t buf[FDCAN_ELEMENT_WORDS];

    memset(&msg, 0, sizeof(msg));
    msg.id    = 0x100U;
    msg.dlc   = 0U;
    msg.flags = 0x2U;   /* RTR bit */

    FDCAN_Codec_PackTxElement(buf, &msg);

    /* T0: [30] RTR = 1, ID 0x100 << 18 = 0x04000000, total = 0x44000000 */
    fails += expect_eq_u32(0x44000000UL, buf[0], "T0 RTR flag");

    return fails;
}

/* =========================================================== */
/*  RX element unpacking                                       */
/* =========================================================== */

static int test_unpack_standard_id(void)
{
    int fails = 0;
    FDCAN_Message_t msg;
    /* Build a synthetic RX element for ID 0x7FF, DLC 4, data 1..4 */
    uint32_t buf[FDCAN_ELEMENT_WORDS];
    buf[0] = (0x7FFUL << 18U);      /* STDID = 0x7FF, XTD=0, RTR=0 */
    buf[1] = 4U;                    /* DLC = 4 */
    buf[2] = 0x04030201UL;          /* bytes 0-3 = 1,2,3,4 */
    buf[3] = 0x08070605UL;          /* bytes 4-7 = 5,6,7,8 */

    FDCAN_Codec_UnpackRxElement(buf, &msg);

    fails += expect_eq_u32(0x7FFUL, msg.id, "RX std ID");
    fails += expect_eq_u8(4U, msg.dlc, "RX DLC");
    fails += expect_eq_u8(0U, msg.flags, "RX flags (std, data)");
    fails += expect_eq_u8(1U, msg.data[0], "RX data[0]");
    fails += expect_eq_u8(4U, msg.data[3], "RX data[3]");
    fails += expect_eq_u8(5U, msg.data[4], "RX data[4]");
    fails += expect_eq_u8(8U, msg.data[7], "RX data[7]");

    return fails;
}

static int test_pack_unpack_roundtrip(void)
{
    int fails = 0;
    FDCAN_Message_t tx, rx;
    uint32_t buf[FDCAN_ELEMENT_WORDS];

    memset(&tx, 0, sizeof(tx));
    tx.id      = 0x456U;
    tx.dlc     = 8U;
    tx.flags   = 0U;
    tx.data[0] = 0x11U;
    tx.data[1] = 0x22U;
    tx.data[2] = 0x33U;
    tx.data[3] = 0x44U;
    tx.data[4] = 0x55U;
    tx.data[5] = 0x66U;
    tx.data[6] = 0x77U;
    tx.data[7] = 0x88U;

    FDCAN_Codec_PackTxElement(buf, &tx);
    FDCAN_Codec_UnpackRxElement(buf, &rx);

    fails += expect_eq_u32(tx.id,    rx.id,    "RT id");
    fails += expect_eq_u8(tx.dlc,   rx.dlc,   "RT dlc");
    fails += expect_eq_u8(tx.flags, rx.flags, "RT flags");
    int i;
    for (i = 0; i < 8; i++) {
        char label[32];
        (void)snprintf(label, sizeof(label), "RT data[%d]", i);
        fails += expect_eq_u8(tx.data[i], rx.data[i], label);
    }

    return fails;
}

/* =========================================================== */
/*  main                                                       */
/* =========================================================== */

int main(void)
{
    int fails = 0;

    fails += test_dlc_to_length();
    fails += test_length_to_dlc();
    fails += test_classic_validation();
    fails += test_pack_standard_id();
    fails += test_pack_rtr_flag();
    fails += test_unpack_standard_id();
    fails += test_pack_unpack_roundtrip();

    if (fails == 0) {
        printf("All fdcan_codec tests passed.\n");
        return 0;
    }

    fprintf(stderr, "%d test check(s) failed.\n", fails);
    return 1;
}
