# Architecture

## Layer Overview

```mermaid
graph TD
    App["App layer\n(main.cpp, superloop)"]
    Driver["Driver layer\n(uart1, spi1/2, fdcan1)"]
    Codec["Codec helpers\n(fdcan_codec — host-testable)"]
    CMSIS["CMSIS / device headers\n(stm32g0b1xx.h, core_cm0plus.h)"]
    HW["STM32G0B1 hardware\n(GPIO, UART, SPI, FDCAN, TIM)"]

    App --> Driver
    App --> Codec
    Driver --> Codec
    Driver --> CMSIS
    CMSIS --> HW
```

Codec helpers (`fdcan_codec.c/h`) depend only on `<stdint.h>` and are fully testable on a host build without MCU headers.

## Directory Structure

```
stm32g071-pilot/
├── App/
│   ├── Inc/main.h          # Pin and clock constants
│   └── Src/main.cpp        # Entry point, superloop, peripheral init
├── Driver/
│   ├── Inc/
│   │   ├── fdcan_codec.h   # FDCAN_Message_t + pack/unpack API
│   │   ├── fdcan1.h        # FDCAN1 public driver API
│   │   ├── spi1.h / spi2.h
│   │   ├── uart1.h / uart2.h
│   │   └── systick.h
│   └── Src/
│       ├── fdcan_codec.c   # DLC helpers, TX/RX element codec
│       ├── fdcan1.c        # FDCAN1 init, TX, RX ISR
│       ├── spi1.c / spi2.c
│       ├── uart1.c / uart2.c
│       ├── syscalls.c      # Newlib stubs (_write → UART2)
│       ├── startup_stm32g071xx.s
│       └── system_stm32g0xx.c  # SystemClock_Config (HSI16 → PLL → 64 MHz)
├── tests/
│   ├── CMakeLists.txt      # Host CTest build (native GCC)
│   └── fdcan_codec_tests.c
├── thirdparty/CMSIS/       # ARM CMSIS + ST device headers
├── cmake/
│   └── toolchain-arm-none-eabi.cmake
├── ld/STM32G0B1RETx_FLASH.ld
├── openocd/stm32g0b1.cfg
└── CMakeLists.txt          # Cross-compile firmware build
```

## Build Targets

| Target | Command | Description |
|--------|---------|-------------|
| Firmware (ELF/HEX/BIN) | `cmake --build build` | Cross-compiled for Cortex-M0+ |
| Host tests | `cmake --build build-host-tests && ctest --test-dir build-host-tests` | Native GCC, no MCU |

## Clock Tree

```mermaid
graph LR
    HSI16["HSI16 (16 MHz)"] -->|M=1| PLL
    PLL -->|N=8, R=2| SYSCLK["SYSCLK 64 MHz"]
    SYSCLK --> AHB["AHB 64 MHz"]
    AHB --> APB1["APB1 64 MHz\n(UART2, SPI1, FDCAN1)"]
    AHB --> APB2["APB2 64 MHz\n(UART1, SPI2, TIM1)"]
```

## Interrupt Priority Map

| IRQ | Handler | Source |
|-----|---------|--------|
| USART1_IRQn | USART1_IRQHandler | UART1 RX byte |
| USART2_IRQn | USART2_IRQHandler | UART2 RX byte |
| SPI1_IRQn | SPI1_IRQHandler | SPI1 TX/RX |
| DMA1_Channel1_IRQn | DMA1_Channel1_IRQHandler | SPI2 DMA complete |
| FDCAN1_IT0_IRQn | FDCAN1_IT0_IRQHandler | FDCAN1 RX FIFO 0 new message |

All interrupts use default NVIC priority (0).
