# stm32g071-pilot

Bare-metal LL-based superloop firmware for **NUCLEO-G071RB**.

## Prerequisites

| Tool | Version |
|------|---------|
| `arm-none-eabi-gcc` | ≥ 10.x |
| `cmake` | ≥ 3.20 |
| `ninja` (recommended) or `make` | any |
| `openocd` / `st-flash` | optional (flash/debug targets) |

## Project structure

```
stm32g071-pilot/
├── cmake/                   # Toolchain and module files
├── Core/                    # Application sources and headers
│   ├── Inc/
│   └── Src/
├── ld/                      # Linker scripts
├── thirdparty/              # Read-only vendor code (CMSIS, LL drivers)
│   ├── CMSIS/
│   └── STM32G0xx_LL_Driver/
└── CMakeLists.txt
```

## Build

```bash
cmake -B build \
      -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake \
      -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Artifacts are written to `build/`:

| File | Description |
|------|-------------|
| `stm32g071-pilot.elf` | Debuggable ELF image |
| `stm32g071-pilot.hex` | Intel HEX for flash tools |
| `stm32g071-pilot.bin` | Raw binary |

## Flash

Using `st-flash`:
```bash
st-flash write build/stm32g071-pilot.bin 0x08000000
```

Using OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32g0x.cfg \
        -c "program build/stm32g071-pilot.elf verify reset exit"
```

## Peripherals

| Peripheral | Pin (NUCLEO-G071RB default) | Notes |
|------------|----------------------------|-------|
| LED (LD4)  | PA5 | Green user LED |
| USART1 TX  | PA9 | AF1 |
| USART1 RX  | PA10 | AF1 |
| USART2 TX  | PA2 | AF1 — printf target |
| USART2 RX  | PA3 | AF1 |
| SPI1 SCK   | PA5 | AF0 |
| SPI1 MISO  | PA6 | AF0 |
| SPI1 MOSI  | PA7 | AF0 |
| CAN/FDCAN  | See `Core/Inc/can.h` | Verify hardware availability |

## Serial (printf)

`printf` is redirected to **USART2** (PA2/PA3).  
Connect at **115200 8N1**.
