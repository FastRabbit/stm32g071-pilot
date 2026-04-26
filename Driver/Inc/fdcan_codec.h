#ifndef FDCAN_CODEC_H
#define FDCAN_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * CAN DLC conversion helpers.
 * Supports classic CAN lengths (0..8) and CAN FD DLC mapping.
 */
uint8_t FDCAN_Codec_DlcToLength(uint8_t dlc);
uint8_t FDCAN_Codec_LengthToDlc(uint8_t length);
int FDCAN_Codec_IsClassicLengthValid(uint8_t length);
int FDCAN_Codec_IsClassicDlcValid(uint8_t dlc);

#ifdef __cplusplus
}
#endif

#endif /* FDCAN_CODEC_H */
