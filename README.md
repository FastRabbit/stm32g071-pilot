# stm32g071-pilot

Bare-metal superloop firmware for **NUCLEO-G0B1RET6**.

## Documentation

- [Documentation index](docs/README.md)
- [First steps](docs/first-steps.md)
- [Development workflow](docs/development-workflow.md)

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
├── App/                     # Application-layer code (main loop, app config)
│   ├── Inc/
│   └── Src/
├── Driver/                  # Low-level driver and platform code
│   ├── Inc/
│   └── Src/
├── tests/                   # Host-side unit tests (CTest)
├── cmake/                   # Toolchain and module files
├── ld/                      # Linker scripts
├── thirdparty/              # Read-only vendor code (CMSIS)
│   └── CMSIS/
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

## Unit Tests (host)

Host tests run natively (macOS/Linux/Windows) and do not require STM32 hardware.

```bash
cmake -S tests -B build-host-tests -DCMAKE_BUILD_TYPE=Debug
cmake --build build-host-tests
ctest --test-dir build-host-tests --output-on-failure
```

Current test coverage includes CAN DLC/length conversion helpers in
`Driver/Src/fdcan_codec.c`.

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

| Peripheral | Pin (NUCLEO-G0B1RET6) | Notes |
|------------|----------------------------|-------|
| LED (LD4)  | PA5 | Green user LED |
| USART1 TX  | PA9 | AF1 |
| USART1 RX  | PA10 | AF1 |
| USART2 TX  | PA2 | AF1 — printf target |
| USART2 RX  | PA3 | AF1 |
| SPI1 SCK   | PB3 | AF0 |
| SPI1 MISO  | PB4 | AF0 |
| SPI1 MOSI  | PB5 | AF0 |
| SPI2 SCK   | PB13 | AF0 |
| SPI2 MISO  | PB14 | AF0 |
| SPI2 MOSI  | PB15 | AF0 |
| FDCAN1 RX  | PB8 | AF3 |
| FDCAN1 TX  | PB9 | AF3 |
| CAN/FDCAN  | See `Driver/Inc/fdcan1.h` | Driver scaffold with IRQ hook |

## Serial (printf)

`printf` is redirected to **USART2** (PA2/PA3).  
Connect at **115200 8N1**.
