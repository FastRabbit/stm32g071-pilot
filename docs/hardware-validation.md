# Hardware Validation Checklist

Board target: NUCLEO-G0B1RET6

Use this checklist after peripheral changes.

## Preparation

1. Connect board over USB (ST-Link).
2. Verify tool access:
   - openocd available
   - debug/flash config present
3. Build firmware successfully.
4. Flash latest ELF/BIN.

## Core Bring-Up Checks

1. Boot reaches main loop.
2. LED toggles with expected cadence.
3. No immediate hard fault/reset loop.

## UART Checks

1. Open serial terminal on USART2 settings.
2. Confirm boot messages are readable.
3. Confirm periodic log messages continue.

Expected baseline:
- 115200 8N1 on USART2

## SPI Checks

1. Verify SPI1 init path does not fault.
2. Verify SPI2 init path does not fault.
3. Run transfer smoke path (loopback or known slave).
4. Confirm completion callback/flag behavior.

## FDCAN Checks

1. Verify FDCAN1 init completes.
2. Confirm IRQ handler triggers on received frame.
3. Validate received ID/DLC/payload values.
4. Validate transmit path behavior when bus is available.

Recommended setups:
- Internal loopback for early bring-up where possible.
- Two-node CAN test for realistic bus validation.

## Debug Checks

1. Start debugger and halt at main.
2. Step through init sequence once.
3. Verify key register fields after init.
4. Resume and monitor runtime stability.

## Pass Criteria

- Firmware boots reliably.
- UART output stable and continuous.
- SPI smoke checks pass.
- FDCAN send/receive path works for at least one known frame.
- No unexpected faults during sustained run.

## Report Template

- Date:
- Commit hash:
- Board:
- Tools (OpenOCD/extension versions):
- Checks executed:
- Result summary:
- Issues found:
- Follow-up actions:
