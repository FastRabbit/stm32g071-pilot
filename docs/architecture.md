# Architecture

This project is intentionally split into clear layers.

## Layering Rules

- App layer orchestrates system behavior and sequencing.
- Driver layer owns direct register/peripheral access.
- App can call Driver APIs.
- Driver must not depend on App symbols.
- Tests should target pure logic and helper modules when possible.

## Folder Responsibilities

- App/Inc, App/Src
  - Application entry and superloop behavior.
  - High-level startup sequence and orchestration.

- Driver/Inc, Driver/Src
  - Peripheral drivers (UART, SPI, FDCAN).
  - Platform/system code (startup, system clock, syscalls).

- tests
  - Host-side tests runnable without target hardware.

- thirdparty
  - Vendor/CMSIS code treated as read-only input.

## Dependency Direction

Allowed:
- App -> Driver
- App -> thirdparty (indirectly via Driver headers where needed)
- Driver -> thirdparty
- tests -> Driver helper modules

Avoid:
- Driver -> App
- tests -> hardware-only side effects

## Build Targets

- Firmware target
  - Cross-compiled with arm-none-eabi toolchain.
  - Produces ELF/HEX/BIN artifacts.

- Host test target
  - Native build using CTest.
  - Focuses on deterministic pure logic.

## Suggested Evolution

1. Keep register-touching code minimal and concentrated in Driver.
2. Move parsers/packers/validation logic into testable helper modules.
3. Use narrow headers in Driver/Inc to keep interfaces stable.
4. Expand tests around helper modules before hardware integration.
