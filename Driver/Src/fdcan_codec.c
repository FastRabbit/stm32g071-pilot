#include "fdcan_codec.h"

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
