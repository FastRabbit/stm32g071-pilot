# Testing Strategy

This project uses two complementary test modes.

## 1. Host Tests (Fast)

Purpose:
- Validate deterministic logic quickly on developer machine and CI.

Scope:
- Codecs/parsers/validators
- Non-hardware state transitions
- Boundary and error handling

Run:

- cmake -S tests -B build-host-tests -DCMAKE_BUILD_TYPE=Debug
- cmake --build build-host-tests
- ctest --test-dir build-host-tests --output-on-failure

Conventions:
- Keep tests independent and deterministic.
- Prefer pure functions and explicit inputs/outputs.
- Name tests by behavior, not implementation detail.

## 2. Hardware Validation (Real Board)

Purpose:
- Verify register configuration and peripheral interactions.

Scope:
- Clock and peripheral init correctness
- Interrupt behavior
- Real data path checks on UART/SPI/FDCAN

Conventions:
- Keep a repeatable smoke-test checklist.
- Log expected serial output and timing behavior.
- Record board setup used for each run.

## What to Test Where

Host tests:
- Conversion logic
- Frame packing/unpacking helper logic
- Input validation functions

Hardware tests:
- Actual peripheral timings and signaling
- IRQ wiring correctness
- Bus-level communication with real devices

## Definition of Healthy Change

Before merge:
1. Firmware build passes.
2. Host tests pass.
3. Affected behavior is documented.
4. Hardware smoke checks run for peripheral-level changes.
