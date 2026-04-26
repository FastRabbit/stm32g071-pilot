/**
 * @file stm32g0xx.h
 * @brief Top-level STM32G0xx family include — selects device header based on
 *        the STM32G0B1xx or STM32G071xx define set by the build system.
 */
#ifndef STM32G0XX_H
#define STM32G0XX_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32G0B1xx)
#include "stm32g0b1xx.h"
#elif defined(STM32G071xx)
#include "stm32g071xx.h"
#else
#error "Please define STM32G0B1xx or STM32G071xx (or another variant) in your build system."
#endif

#ifdef __cplusplus
}
#endif

#endif /* STM32G0XX_H */
