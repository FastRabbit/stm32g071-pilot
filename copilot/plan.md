# Project Plan

Status: Active
Version baseline: v0.1
Last updated: 2026-04-26

## Current State
- App and driver code are separated into App and Driver folders.
- Firmware cross-build is passing.
- Host unit test framework is in place with CTest.
- Initial FDCAN codec unit tests are passing.

## Near-Term Milestones

1. Complete FDCAN1 TX path
- Implement actual TX element write to message RAM.
- Add bounds and format validation.
- Add host-side helper tests for frame packing logic.

2. Complete FDCAN1 RX path
- Implement FIFO0 message extraction in IRQ handler.
- Convert register-level payload into FDCAN_Message_t.
- Invoke registered callback with parsed message.

3. Add FDCAN message RAM abstraction
- Create a small codec/packing layer for TX and RX elements.
- Keep hardware access thin and testable.

4. Expand host unit tests
- Add tests for DLC, ID, and flags conversion edge cases.
- Add negative tests for malformed inputs.
- Keep tests hardware-independent and fast.

5. Hardware verification on NUCLEO-G0B1RET6
- Validate UART, SPI, and FDCAN init sequence.
- Run CAN loopback or two-node smoke tests.
- Confirm no hard faults in long-run superloop.

6. Developer workflow polish
- Optionally track shared VS Code configs if team-wide use is desired.
- Add CI job for host tests and firmware build.

## Definition of Done for v0.2
- FDCAN1 TX and RX are functionally complete.
- At least one hardware CAN send/receive smoke test passes.
- Host tests include codec and frame pack/unpack checks.
- Build and tests documented in README.
