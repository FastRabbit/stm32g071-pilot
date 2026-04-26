#include <stdio.h>

#include "fdcan_codec.h"

static int expect_eq_u8(unsigned int expected, unsigned int actual, const char *what)
{
    if (expected != actual) {
        fprintf(stderr, "FAIL: %s expected=%u actual=%u\n", what, expected, actual);
        return 1;
    }
    return 0;
}

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

int main(void)
{
    int fails = 0;

    fails += test_dlc_to_length();
    fails += test_length_to_dlc();
    fails += test_classic_validation();

    if (fails == 0) {
        printf("All fdcan_codec tests passed.\n");
        return 0;
    }

    fprintf(stderr, "%d test checks failed.\n", fails);
    return 1;
}
