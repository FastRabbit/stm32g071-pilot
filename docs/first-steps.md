# First Steps

This guide gets a fresh clone to first firmware build, first host tests, and first debug session.

## 1. Prerequisites

- arm-none-eabi-gcc
- cmake >= 3.20
- openocd
- VS Code extensions:
  - CMake Tools
  - Cortex-Debug
  - C/C++

## 2. Clone and Build Firmware

1. Clone and enter repo.
2. Configure firmware build:

   cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Debug

3. Build:

   cmake --build build

Expected outputs in build:
- stm32g071-pilot.elf
- stm32g071-pilot.hex
- stm32g071-pilot.bin

## 3. Run Host Unit Tests

1. Configure host tests:

   cmake -S tests -B build-host-tests -DCMAKE_BUILD_TYPE=Debug

2. Build tests:

   cmake --build build-host-tests

3. Run tests:

   ctest --test-dir build-host-tests --output-on-failure

## 4. Flash and Debug

- Flash task or command:

  openocd -f openocd/stm32g0b1.cfg -c "program build/stm32g071-pilot.elf verify reset exit"

- Debug in VS Code:
  - Start configuration: Debug with ST-Link
  - Halt at main, then step and inspect peripherals/registers

## 5. Project Layout

- App: application entry and orchestration
- Driver: low-level peripherals and platform code
- tests: host-side unit tests
- thirdparty: CMSIS and vendor headers

## 6. Common Issues

- OpenOCD not found: install and ensure in PATH
- Build cache mismatch: remove build folder and reconfigure
- Debug connect errors: verify ST-Link cable and board power
