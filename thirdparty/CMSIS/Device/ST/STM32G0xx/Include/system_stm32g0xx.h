/**
 * @file system_stm32g0xx.h
 * @brief CMSIS System header for STM32G0xx.
 */
#ifndef SYSTEM_STM32G0XX_H
#define SYSTEM_STM32G0XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint32_t SystemCoreClock;

void SystemInit(void);
void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_STM32G0XX_H */
