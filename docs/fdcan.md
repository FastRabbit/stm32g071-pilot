# FDCAN Notes

This document tracks current FDCAN status and next implementation steps.

## Current State

- FDCAN1 init exists in Driver/Src/fdcan1.c.
- IRQ hook exists for FDCAN1_IT0.
- Send API exists but TX buffer write is still incomplete.
- RX FIFO extraction path is still incomplete.
- Helper codec exists in Driver/Src/fdcan_codec.c with host tests.

## Data Model

- Public message type: FDCAN_Message_t
  - id
  - dlc
  - data[8]
  - flags

- Codec helper functions
  - DLC to length conversion
  - Length to DLC conversion
  - Classic CAN DLC/length validation

## Recommended TX Path

1. Validate input message pointer and DLC.
2. Check TX queue availability.
3. Build TX element (ID, frame control, DLC, payload).
4. Write element to message RAM.
5. Trigger TX request for selected buffer/queue slot.
6. Return success/failure code.

## Recommended RX Path

1. Enter IRQ handler and check RX FIFO new-message flag.
2. Read FIFO get-index and message element.
3. Decode ID, DLC, flags, payload bytes.
4. Advance FIFO acknowledge index.
5. Invoke registered callback with parsed message.
6. Clear interrupt flags.

## Safety and Robustness

- Validate all external inputs.
- Keep ISR work minimal and bounded.
- Prefer copying message data out quickly in ISR.
- Avoid hidden state transitions without explicit flags.

## Test Strategy

Host-side:
- Keep adding tests for frame packing and parsing helpers.
- Add edge-case tests for DLC/length boundaries.

Hardware-side:
- Loopback check.
- Two-node smoke test at configured bitrate.
- Verify callback path and payload integrity.

## Open TODOs

- Implement concrete TX message RAM writes.
- Implement concrete RX FIFO parsing and acknowledge.
- Add tests for pack/unpack helper routines once extracted.
