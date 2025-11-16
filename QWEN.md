# PEW|FORMER Eurorack Sequencer Firmware

## Project Overview

PEW|FORMER is an open-source firmware for the PER|FORMER eurorack sequencer module, originally created by westlicht and now maintained with improvements from the community. This is a professional embedded systems project combining real-time sequencer logic on STM32 hardware with cross-platform simulation capabilities.

### Key Facts
- **Language**: C++11 with some C for low-level drivers
- **Platform**: STM32F405RGT6 (ARM Cortex-M4, 168 MHz, 1MB Flash, 192KB RAM)
- **RTOS**: FreeRTOS for task management
- **Build System**: CMake + Make
- **Lines of Code**: ~130k C/C++
- **License**: MIT

### Hardware Capabilities
- 8 CV/Gate output pairs for sequencing
- 256x64 OLED display (4bpp)
- 4 CV inputs (12-bit ADC)
- 8 CV outputs (16-bit DAC)
- MIDI In/Out
- USB Host for MIDI controllers
- SD Card storage
- Button/LED matrix (8×5 buttons, 8×4 bi-color LEDs)
- Encoder with pushbutton

---

## Codebase Structure

### Top-Level Directory Organization

```
performer-ph/
├── .github/workflows/       # CI/CD (GitHub Actions)
├── .vscode/                 # VS Code configuration
├── build/                   # Build outputs (gitignored)
├── cmake/                   # CMake toolchain files
├── doc/                     # Design documents
├── resources/               # Font generation scripts
├── scripts/                 # Deployment and flashing scripts
├── src/                     # Main source code
├── tools/                   # Downloaded ARM toolchain and OpenOCD
├── downloads/               # Downloaded dependencies
├── Makefile                 # Top-level build orchestration
├── CMakeLists.txt          # Root CMake configuration
├── ACCUMULATOR_IMPLEMENTATION_PLAN.md # Accumulator feature implementation
├── ACCUMULATOR_MVP_STATUS.md # Accumulator MVP status
├── ACCUMULATOR_STEPS.md     # Accumulator implementation steps
├── CHANGELOG.md             # Version history and release notes
├── CLAUDE.md                # AI assistant guide
├── FREE-TRACKS.md           # Independent BPM + 64-note scales
├── TOP-LEVEL-PLAN.md        # Feature implementation roadmap
├── VS_MEBITEK.md            # Feature comparison with mebitek fork
└── README.md               # Developer setup guide
```

### Source Code Structure (`src/`)

```
src/
├── apps/                       # Applications
│   ├── bootloader/            # Bootloader for firmware updates
│   ├── sequencer/             # Main sequencer application
│   │   ├── engine/            # Sequencing engine (arpeggiator, tracks)
│   │   ├── model/             # Data model and state management
│   │   ├── python/            # Python bindings for testing
│   │   ├── tests/             # Python integration tests
│   │   ├── asteroids/         # Easter egg game
│   │   ├── intro/             # Boot animation
│   │   └── ui/                # User interface
│   │       ├── controllers/   # Launchpad controller support
│   │       ├── pages/         # UI pages/screens
│   │       └── painters/      # Graphics rendering
│   ├── tester/                # Hardware testing application
│   └── hwconfig/              # Hardware configuration headers
│
├── core/                       # Shared core libraries
│   ├── fs/                    # File system abstraction
│   ├── gfx/                   # Graphics primitives and fonts
│   ├── io/                    # Serialization (VersionedReader/Writer)
│   ├── math/                  # Vector/matrix math (Vec2-4, Mat3-4)
│   ├── midi/                  # MIDI message parsing
│   ├── profiler/              # Performance profiling
│   └── utils/                 # Containers, string, random, etc.
│
├── libs/                       # Bundled third-party libraries
│   ├── stb/                   # stb_sprintf, stb_image_write
│   └── ff/                    # FatFs file system
│
├── platform/                   # Platform-specific code
│   ├── stm32/                 # STM32F4 hardware platform
│   │   ├── drivers/           # Hardware drivers (ADC, DAC, LCD, etc.)
│   │   ├── hal/               # Hardware abstraction layer
│   │   ├── os/                # FreeRTOS integration
│   │   └── libs/              # libopencm3, FreeRTOS, libusbhost
│   │
│   └── sim/                    # Simulator platform (SDL2-based)
│       ├── sim/               # Simulator core
│       ├── drivers/           # Virtual hardware drivers
│       ├── frontend/          # SDL2 graphics and UI
│       └── libs/              # soloud, nanovg, pybind11, rtmidi
│
├── test/                       # Test infrastructure
├── tests/                      # Test implementations
│   ├── unit/                  # Unit tests (core, sequencer logic)
│   └── integration/           # Integration tests (drivers, fs)
│
└── SystemConfig.h             # Global compile-time configuration
```

---

## Building and Running

### Prerequisites

**macOS/Linux:**
- CMake 3.0+
- Git with submodule support
- For simulator: SDL2, SDL2_image, SDL2_ttf, Python3
- For Linux: `sudo apt-get install libtool autoconf cmake libusb-1.0.0-dev libftdi-dev pkg-config`

**Windows:**
- Use Vagrant (see README.md)

### Initial Setup

```bash
# Clone with submodules (CRITICAL!)
git clone --recursive https://github.com/djphazer/performer.git
cd performer

# Install ARM toolchain and OpenOCD (for hardware builds)
make tools_install  # Downloads ~100MB, takes a few minutes

# Setup build directories
make setup_stm32    # For hardware builds
make setup_sim      # For simulator builds
make setup_www      # For WebAssembly builds (requires emsdk)
```

### Build Directory Structure

After setup, you'll have:

```
build/
├── stm32/
│   ├── debug/       # Hardware build with debug symbols
│   └── release/     # Optimized hardware build (use this for releases)
└── sim/
    ├── debug/       # Simulator with debug info (use for development)
    ├── release/     # Optimized simulator
    └── www/         # WebAssembly build
```

### Building

**For Hardware (STM32):**
```bash
cd build/stm32/release
make -j sequencer              # Main sequencer with bootloader support
make -j sequencer_standalone   # Sequencer without bootloader
make -j bootloader             # Bootloader only
make -j tester                 # Hardware tester
```

**For Simulator:**
```bash
cd build/sim/debug
make -j sequencer
./src/apps/sequencer/sequencer  # Run simulator (must run from build dir)
```

**For Testing:**
```bash
cd build/sim/release
make test
```

### Flashing Hardware

```bash
cd build/stm32/release
make flash_bootloader  # Flash bootloader first
make flash_sequencer   # Flash sequencer application
```

**JTAG Configuration:**
- Default: Olimex ARM-USB-OCD-H
- To change: Edit `OPENOCD_INTERFACE` in `src/platform/stm32/CMakeLists.txt`
- Available interfaces: `tools/openocd/share/openocd/scripts/interface/`

### Build Artifacts

For each application (e.g., `sequencer`), the build produces:
- `sequencer` - ELF binary with debug symbols
- `sequencer.bin` - Raw binary
- `sequencer.hex` - Intel HEX format (for flashing)
- `sequencer.srec` - Motorola SREC format
- `sequencer.list` - Full disassembly
- `sequencer.map` - Symbol/section map
- `sequencer.size` - Section sizes
- `UPDATE.DAT` - Bootloader update file (sequencer only)

---

## Development Conventions

### Style Guide

**Indentation:**
- C/C++ files: 4 spaces
- CMake files: 4 spaces
- Makefiles: Tabs

**Line Endings:**
- Always use LF (Unix-style), never CRLF
- Enforced by `.editorconfig`

**Naming:**
- Classes: PascalCase (e.g., `NoteSequence`)
- Functions/methods: camelCase (e.g., `processStep()`)
- Constants: UPPER_SNAKE_CASE (e.g., `CONFIG_CPU_FREQUENCY`)
- Files: Match class names or descriptive lowercase

**Code Organization:**
- Header files (.h) for interface definitions
- Implementation files (.cpp) for implementation
- Use `#pragma once` for include guards
- Avoid deeply nested code; prefer early returns

### Architecture Patterns

**Layered Architecture:**
```
Applications (sequencer, bootloader, tester)
         ↓
    Core Libraries (fs, gfx, midi, utils)
         ↓
   OS Abstraction Layer
         ↓
Platform Drivers (STM32 or Simulator)
         ↓
    Hardware / SDL2
```

**Model-View-Controller:**
- Used extensively in sequencer UI
- `model/` - Application state and business logic
- `ui/pages/` - View layer (displays)
- `ui/controllers/` - Controller layer (input handling)

**Hardware Abstraction:**
- Code in `apps/` and `core/` should be platform-independent
- Platform-specific code stays in `platform/stm32/` or `platform/sim/`
- Use virtual interfaces for drivers

### Memory Management

**Critical Constraints:**
- 192KB total RAM on STM32
- No dynamic allocation in real-time code paths
- Use object pools and fixed-size buffers
- CCMRAM (64KB) for fast, non-DMA data
- SRAM for large application state and DMA buffers

**Best Practices:**
- Prefer stack allocation for small objects
- Use `ObjectPool<T>` for pooled allocation
- Use `Container<T, N>` for fixed-capacity containers
- Avoid STL containers that allocate (use sparingly)

---

## Key Technologies & Dependencies

### Third-Party Libraries

**Git Submodules (in `src/platform/*/libs/`):**
- **libopencm3** - STM32F4 peripheral library (Oct 2024)
- **FreeRTOS** - Real-time operating system
- **libusbhost** - USB host driver for MIDI controllers
- **NanoVG** - Vector graphics rendering (simulator)
- **SoLoud** - Audio engine (simulator)
- **pybind11** - Python C++ bindings for testing
- **RtMidi** - Cross-platform MIDI I/O (simulator)

**Bundled Libraries:**
- **FatFs** - SD card file system
- **stb_sprintf** - Lightweight printf
- **stb_image_write** - Image export
- **args** - Command-line parsing (simulator)
- **tinyformat** - String formatting

### Toolchain

- **ARM GCC 14.2** - Downloaded by `make tools_install`
- **OpenOCD 0.10.0** - For JTAG flashing
- **Emscripten 3.1.74** - For WebAssembly builds (CI only)

---

## Current Development Focus

### Active Feature Development

The project is currently focused on implementing two major enhancements:

1. **Independent Track BPM** - Allow each of the 8 tracks to run at different tempo multipliers relative to the master clock, enabling complex polyrhythmic patterns

2. **Free-Form Scales** - Extend user scales to 64 notes with optional non-repeating octave behavior for microtonal and experimental music

### Implementation Plans

**Key Documents:**
- `TOP-LEVEL-PLAN.md` - Comprehensive feature implementation roadmap
- `FREE-TRACKS.md` - Independent BPM and 64-note scales specification
- `ACCUMULATOR_IMPLEMENTATION_PLAN.md` - Metropolix-style accumulator functionality
- `CLAUDE.md` - AI assistant guide with detailed architecture information

**Current Status:**
- Independent Track BPM: Planned for implementation
- Accumulator: Detailed implementation plan ready
- 64-Note Scales: Conditional on UI prototype validation
- Mebitek features: Not recommended (conflicts with project philosophy)

---

## Testing

### Unit Tests

**Location:** `src/tests/unit/`

**Categories:**
- Core I/O serialization tests
- Utility function tests (string, random, containers)
- Sequencer logic tests (scales, curves)

**Running:**
```bash
cd build/sim/release
make test
```

### Integration Tests

**Location:** `src/tests/integration/`

**Coverage:**
- Hardware driver simulation
- File system operations
- Driver-level functionality

### Python Tests

**Location:** `src/apps/sequencer/tests/`

**Purpose:**
- UI workflow automation
- End-to-end testing via Python bindings
- Screenshot generation

**Running:**
```bash
cd build/sim/debug
./src/apps/sequencer/sequencer --test path/to/test.py
```

---

## Common Workflows

### Adding a New Feature

1. **Plan the change:**
   - Identify affected layers (model, engine, UI)
   - Check if changes need to be version-controlled (data model)
   - Review existing similar features

2. **Implement:**
   - Start with data model changes in `model/`
   - Add engine logic in `engine/` if needed
   - Create/modify UI pages in `ui/pages/`
   - Update painters in `ui/painters/` for graphics

3. **Test:**
   - Build and test in simulator first
   - Write unit tests if adding core functionality
   - Test on hardware if touching drivers or performance-critical code

4. **Document:**
   - Update relevant `.md` files in `doc/` if significant
   - Add entry to `CHANGELOG.md`

### Debugging

**Simulator (Preferred for Development):**
```bash
cd build/sim/debug
gdb ./src/apps/sequencer/sequencer
# Or use VS Code with provided launch.json
```

**Hardware:**
```bash
cd build/stm32/debug
make flash_sequencer
# Use OpenOCD + GDB for debugging
# See .vscode/launch.json for GDB configuration
```

**Profiler:**
- Enable in `SystemConfig.h`: `#define CONFIG_ENABLE_PROFILER 1`
- Access profiler output on serial console

### Modifying the Data Model

**CRITICAL:** Data model changes affect project file compatibility!

1. Update model classes in `src/apps/sequencer/model/`
2. Update version number in model serialization code
3. Implement version migration in `VersionedReader`
4. Test loading old project files
5. Document breaking changes in `CHANGELOG.md`

### Working with MIDI Controllers

**Supported Devices:**
- Novation Launchpad (S, Mini, Mk2, Pro, Mini Mk3, X, Pro Mk3)

**Code Location:**
- `src/apps/sequencer/ui/controllers/launchpad/`
- Each controller has a specific implementation

**Adding Support:**
1. Create new controller class inheriting from base
2. Implement device-specific button/LED mappings
3. Register in controller manager
4. Test with physical device

---

## Important Files & Locations

### Configuration

| File | Purpose |
|------|---------|
| `src/SystemConfig.h` | Global compile-time configuration (CPU freq, peripherals) |
| `src/apps/hwconfig/` | Hardware variant configurations (DAC models, etc.) |
| `.editorconfig` | Editor formatting rules |
| `.vscode/` | VS Code settings, launch configs, tasks |

### Documentation

| File | Purpose |
|------|---------|
| `README.md` | Development setup guide |
| `CHANGELOG.md` | Version history and release notes |
| `doc/DesignDocument.md` | Hardware specs, UI design, features |
| `doc/MemoryMap.md` | STM32F4 memory layout |
| `doc/PinMap.md` | Hardware pin configuration |
| `doc/Priorities.md` | Development priorities |
| `doc/improvements/` | Fork-specific feature documentation |

### Build & Deployment

| File | Purpose |
|------|---------|
| `Makefile` | Top-level build orchestration, toolchain setup |
| `CMakeLists.txt` | Root CMake configuration |
| `cmake/arm.cmake` | ARM cross-compilation toolchain |
| `scripts/deploy` | Package hardware artifacts |
| `scripts/deploy-simulator` | Package simulator builds |
| `scripts/flash` | Flash firmware via OpenOCD |
| `scripts/makeupdate` | Generate UPDATE.DAT bootloader file |

### CI/CD

| File | Purpose |
|------|---------|
| `.github/workflows/ci.yml` | GitHub Actions workflow |

**CI Pipeline:**
- Builds for 4 platforms: STM32, Linux sim, macOS sim, WebAssembly
- Runs tests on simulator builds
- Uploads artifacts for every push
- Creates GitHub releases for tags
- Deploys simulator to GitHub Pages

---

## Git & CI/CD Practices

### Branch Strategy

- **master** - Main development branch
- **claude/*** - AI-assisted development branches (temporary)
- Feature branches merged via PRs

### Commit Messages

**Study recent commits for style:**
```bash
git log --oneline -20
```

**Common patterns:**
- `Fix [issue]` - Bug fixes
- `Add [feature]` - New features
- `Update [component]` - Enhancements
- `Refactor [area]` - Code improvements
- `CHORE: [task]` - Maintenance tasks

### Submodule Management

**CRITICAL:** This project uses git submodules!

```bash
# Update all submodules
git submodule update --init --recursive

# Update specific submodule
cd src/platform/stm32/libs/libopencm3
git checkout [ref]
cd ../../../../..
git add src/platform/stm32/libs/libopencm3
git commit -m "Update libopencm3 to [version]"
```

### CI/CD Workflow

**On Every Push:**
1. Build STM32 firmware (release)
2. Build Linux simulator (release, with tests)
3. Build macOS simulator (release, with tests)
4. Build WebAssembly simulator
5. Upload artifacts (non-tag pushes)

**On Tag Push:**
1. All of the above
2. Create GitHub release (draft)
3. Deploy WebAssembly simulator to GitHub Pages

---

## Tips for AI Assistants

### Before Making Changes

1. **Understand the layer:** Is this model, engine, UI, or driver code?
2. **Check dependencies:** What other components depend on this?
3. **Review data model impact:** Will this affect project file format?
4. **Consider memory constraints:** STM32 only has 192KB RAM!
5. **Test in simulator first:** Much faster development cycle

### Common Pitfalls to Avoid

1. **Don't break data model compatibility** without version migration
2. **Don't use dynamic allocation** in real-time code paths
3. **Don't forget about the simulator** - it needs to build too
4. **Don't modify submodules** without explicit approval
5. **Don't ignore memory constraints** - profile and measure
6. **Don't add large dependencies** - embedded platform has strict limits
7. **Don't skip testing** - use the simulator!

### When to Use Each Platform

**Simulator (Preferred):**
- UI development and testing
- Business logic changes
- Python integration tests
- Rapid iteration
- Debugging with standard tools

**Hardware:**
- Driver development
- Performance optimization
- Hardware-specific features
- Real-time behavior verification
- Final testing before release

### Architecture Guidelines

**Platform Independence:**
- Keep application code in `apps/` and `core/` platform-independent
- Use OS abstraction layer for threading, timing, etc.
- Use driver interfaces, not direct hardware access

**Separation of Concerns:**
- Model: Data and business logic
- Engine: Sequencing and real-time processing
- UI: User interaction and display
- Drivers: Hardware abstraction

**Error Handling:**
- Use assertions for programmer errors (`assert()`)
- Use return codes for runtime errors
- Log errors to serial console when possible

### Performance Considerations

**Real-Time Requirements:**
- Audio processing runs at high priority
- UI runs at lower priority
- Avoid blocking operations in real-time tasks
- Use FreeRTOS queues for inter-task communication

**Optimization:**
- Profile before optimizing (`CONFIG_ENABLE_PROFILER`)
- Optimize hot paths identified by profiler
- Use compiler optimization flags (already set in release builds)
- Consider CCMRAM for performance-critical data

### Code Review Checklist

Before submitting changes:

- [ ] Builds successfully for both STM32 and simulator
- [ ] No compiler warnings
- [ ] Tests pass (`make test` in simulator)
- [ ] Code follows existing style conventions
- [ ] No data model changes without version migration
- [ ] Memory usage checked (if applicable)
- [ ] Documentation updated (if needed)
- [ ] CHANGELOG.md updated (for user-visible changes)
- [ ] Tested in simulator
- [ ] Tested on hardware (if touching drivers/performance)

---

## Version History

- **v0.5** (Current) - Updated toolchain (GCC 14.2, libopencm3 Oct 2024)
- **v0.1.42** (June 2022) - Launchpad Mk3 firmware support
- **v0.1.0** (Oct 2018) - Initial release

See `CHANGELOG.md` for detailed version history.

---

## Useful Resources

### Documentation
- Project Website: https://westlicht.github.io/performer
- Hardware Design: https://github.com/westlicht/performer-hardware
- This Fork: https://github.com/djphazer/performer

### Community
- Original Author: westlicht
- Current Maintainer: djphazer (Phazerville)
- Previous Forks: jackpf (improvements), mebitek (features)

### External References
- STM32F405 Reference Manual
- FreeRTOS Documentation
- libopencm3 API Documentation
- CMake Documentation

---

## Quick Reference Commands

```bash
# Setup
git clone --recursive [url]
make tools_install
make setup_stm32 && make setup_sim

# Build
cd build/stm32/release && make -j
cd build/sim/debug && make -j

# Test
cd build/sim/release && make test

# Flash
cd build/stm32/release && make flash_bootloader flash_sequencer

# Run Simulator
cd build/sim/debug && ./src/apps/sequencer/sequencer

# Deploy
make deploy          # Hardware artifacts
make deploy-simulator # Simulator artifacts

# Clean
rm -rf build/
```

---

## Contact & Contribution

For questions, issues, or contributions:
- GitHub Issues: Primary communication channel
- Read `README.md` for contribution guidelines
- Follow existing code patterns
- Test thoroughly before submitting PRs