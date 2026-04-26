# VS Code Build & Debug Setup

## Debugger: ST-Link v2 + OpenOCD + Cortex-Debug

### What You Need

1. **ST-Link v2** (built into NUCLEO-G0B1RET6 board)
2. **OpenOCD** — open-source on-chip debugger
3. **VS Code Extensions:**
   - **Cortex-Debug** (marus25.cortex-debug) — GDB frontend for ARM
   - **CMake Tools** (ms-vscode.cmake-tools) — CMake integration
   - **C/C++ Intellisense** (ms-vscode.cpptools)

### Installation

#### macOS

```bash
brew install openocd
```

#### Linux (Ubuntu/Debian)

```bash
sudo apt-get install openocd
```

#### Windows

Download from [OpenOCD SourceForge](https://sourceforge.net/projects/openocd/files/) or use:

```bash
winget install openocd
```

#### VS Code Extensions

Install via Extensions panel (Ctrl+Shift+X):
- Search "Cortex-Debug" → Install
- Search "CMake Tools" → Install  
- Search "C/C++" → Install (Microsoft)

### Build & Debug Workflow

#### 1. **Configure Project** (first time)

```bash
Ctrl+Shift+B → Select "configure"
```

This runs CMake and generates build files.

#### 2. **Build** (Ctrl+Shift+B → Default)

```bash
Ctrl+Shift+B → Select "build"
```

Or just hit Ctrl+Shift+B (uses "build" as default).

#### 3. **Debug** (F5)

```bash
F5 → Select "Debug with ST-Link"
```

This:
- Builds the project (preLaunchTask)
- Starts OpenOCD
- Connects GDB
- Halts CPU at `main()`
- You can now:
  - Step (F10)
  - Step into (F11)
  - Continue (F5)
  - Set breakpoints (click line numbers)
  - View locals/watch

#### 4. **Flash Only** (if build is skipped)

```bash
Ctrl+Shift+B → Select "flash"
```

Uses OpenOCD to program flash directly.

### Available Tasks

| Task | Shortcut | Purpose |
|------|----------|---------|
| build | Ctrl+Shift+B (default) | Incremental build |
| configure | Ctrl+Shift+B | CMake configure (once per workspace) |
| rebuild | Ctrl+Shift+B | Clean + configure + build |
| clean | Ctrl+Shift+B | Remove build/ folder |
| flash | Ctrl+Shift+B | Program flash via OpenOCD |

### Debugging Commands in Debug Console

While stopped at breakpoint:

```gdb
print SystemCoreClock           # Show variable
monitor reset halt              # Reset & halt MCU
monitor arm semihosting enable  # Enable semihosting (if using)
```

### Troubleshooting

#### "OpenOCD not found"

- **macOS:** `brew install openocd`
- **Linux:** `sudo apt-get install openocd`
- **Windows:** Ensure OpenOCD is in PATH

#### "ST-Link not detected"

- **macOS/Linux:** Likely a udev rule issue. Install STLink rules:
  ```bash
  sudo cp openocd/stlink-udev.rules /etc/udev/rules.d/
  sudo udevadm control --reload
  sudo udevadm trigger
  ```
- **Windows:** Install ST-Link USB driver from ST website

#### "Cortex-Debug not working"

- Verify `openocd/stm32g0b1.cfg` path in `.vscode/launch.json`
- Check OpenOCD output in Debug Console
- Try manual OpenOCD test:
  ```bash
  openocd -f openocd/stm32g0b1.cfg
  ```

### VS Code Keybinds

| Key | Action |
|-----|--------|
| F5 | Start/Continue Debug |
| F10 | Step Over |
| F11 | Step Into |
| Shift+F11 | Step Out |
| Ctrl+Shift+B | Run Build Task |
| Ctrl+K Ctrl+0 | Fold All Regions |

### Notes

- **First build** may take 5-10 seconds (fresh CMake config)
- **Incremental builds** typically < 1 second
- **Debug startup** ~2-3 seconds (OpenOCD init + GDB handshake)
- **Breakpoints** work in both `.c` and `.cpp` files
- Use **Watch** panel to monitor variables during execution

### Why ST-Link + OpenOCD + Cortex-Debug?

| Feature | ST-Link | OpenOCD | Cortex-Debug |
|---------|---------|---------|--------------|
| Cost | Free (on NUCLEO) | Free (open-source) | Free (extension) |
| Reliability | Industrial-grade | Proven (10+ years) | Well-maintained |
| Speed | Fast (4 MHz typical) | N/A | N/A |
| Features | Full SWD/JTAG | Full support | GDB + peripherals |
| Compatibility | STM32 native | All ARM | VS Code native |

### Alternatives

- **J-Link** (Segger) — Faster, more features, cost ~$100
- **pyOCD** — Python-based, simpler setup (slower)
- **Arduino IDE** — Limited debugging, not recommended

Stick with **ST-Link + OpenOCD** for development.
