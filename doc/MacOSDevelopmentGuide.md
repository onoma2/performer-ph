# macOS Development & Testing Guide for PEW|FORMER

**For developers new to STM32 and C/C++ development**

This guide provides a complete step-by-step workflow for developing and testing new features on macOS for the PEW|FORMER eurorack sequencer firmware project.

---

## Table of Contents

1. [Initial Setup](#phase-1-initial-setup-one-time)
2. [Understanding the Codebase](#phase-2-understanding-the-codebase)
3. [Building the Simulator](#phase-3-building-the-simulator)
4. [Development Workflow](#phase-4-development-workflow)
5. [Testing Your Changes](#phase-5-testing-your-changes)
6. [Working with STM32 Hardware](#phase-6-working-with-stm32-hardware-advanced)
7. [Debugging Tips](#phase-7-debugging-tips-for-beginners)
8. [Best Practices](#phase-8-best-practices)
9. [Quick Reference](#quick-reference-commands)

---

## Phase 1: Initial Setup (One-Time)

### Step 1.1: Install Development Tools

```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install SDL2 (required for simulator)
brew install sdl2

# Install CMake (if not already installed)
brew install cmake

# Install Python 3 (usually pre-installed on macOS)
python3 --version  # Verify it's installed
```

### Step 1.2: Clone and Setup Repository

```bash
# Clone with submodules (important!)
git clone --recursive https://github.com/onoma2/performer-ph.git
cd performer-ph

# If you already cloned without --recursive, run:
git submodule update --init --recursive
```

### Step 1.3: Install ARM Toolchain (Optional - for hardware compilation)

```bash
# This installs the ARM cross-compiler needed for STM32 hardware
make tools_install
```

**Note:** Skip this initially if you only want to develop using the simulator. The simulator is much faster for development and doesn't require hardware.

### Step 1.4: Setup Build Directories

```bash
# Setup simulator (recommended for beginners)
make setup_sim

# This creates: build/sim/debug and build/sim/release
```

**What this does:**
- Creates build directories with CMake configuration
- Configures debug builds (with symbols for debugging)
- Configures release builds (optimized for testing)

---

## Phase 2: Understanding the Codebase

### Step 2.1: Project Structure

```
performer-ph/
├── src/
│   ├── apps/
│   │   ├── sequencer/          ← Main application (START HERE!)
│   │   │   ├── engine/         ← Sequencer logic (tracks, patterns, steps)
│   │   │   ├── model/          ← Data structures (Project, Track, Sequence)
│   │   │   ├── ui/             ← User interface code (pages, controllers)
│   │   │   ├── python/         ← Python bindings for testing
│   │   │   └── tests/          ← Python integration tests
│   │   ├── bootloader/         ← System bootloader
│   │   └── tester/             ← Hardware testing app
│   ├── core/                   ← Shared utilities (math, MIDI, utils)
│   ├── platform/
│   │   ├── sim/               ← Simulator implementation (SDL2/OpenGL)
│   │   └── stm32/             ← Hardware-specific code (STM32F4)
│   └── tests/
│       └── unit/              ← C++ unit tests
├── doc/                        ← Documentation (you are here!)
├── build/                      ← Build outputs (created by make setup_*)
└── CMakeLists.txt             ← Root build configuration
```

### Step 2.2: Key Concepts

**What is PEW|FORMER?**
- Firmware for a hardware eurorack sequencer module
- Controls musical sequences, patterns, and CV/gate outputs
- Supports MIDI, step sequencing, curve generation, and more

**Development Approaches:**
1. **Simulator** (recommended): Runs on macOS, fast iteration, no hardware needed
2. **Hardware**: Flash to STM32 chip, test on real hardware

**Technologies Used:**
- **Language:** C++11
- **Build System:** CMake + Make
- **Embedded OS:** FreeRTOS (hardware only)
- **Graphics:** SDL2 + OpenGL (simulator), NanoVG (both)
- **Testing:** Custom C++ framework + Python unittest

### Step 2.3: Read Key Documentation

```bash
# Project overview
cat README.md

# Hardware and feature specifications
cat doc/DesignDocument.md

# Hardware memory layout
cat doc/MemoryMap.md
```

---

## Phase 3: Building the Simulator

### Step 3.1: Build Debug Version (for development)

```bash
cd build/sim/debug
make -j  # -j uses multiple CPU cores for faster builds
```

**What's happening:**
- CMake compiles C++ source files to object files (.o)
- Links object files into libraries and executables
- Typical build time: 1-5 minutes (first time), seconds (incremental)

**Expected output:**
```
[ 12%] Building CXX object src/core/CMakeFiles/core.dir/utils/StringBuilder.cpp.o
[ 24%] Linking CXX static library libcore.a
[ 36%] Building CXX object src/apps/sequencer/CMakeFiles/sequencer.dir/Main.cpp.o
...
[100%] Built target sequencer
```

### Step 3.2: Run the Simulator

```bash
# IMPORTANT: Must run from build directory!
./src/apps/sequencer/sequencer
```

**What you should see:**
- A window opens showing the sequencer interface
- Simulated hardware controls (encoders, buttons)
- OLED display rendering

**Controls:**
- Mouse: Click buttons, turn encoders
- Keyboard: Various shortcuts (see simulator window)
- ESC: Exit simulator

### Step 3.3: Understanding Build Output

**Key files created:**
```
build/sim/debug/
├── src/apps/sequencer/sequencer    ← Main executable
├── src/core/libcore.a              ← Core library
├── src/platform/sim/libplatform.a  ← Platform abstraction
└── CMakeFiles/                     ← Build metadata
```

**Build artifacts:**
- `.o` files: Compiled object code
- `.a` files: Static libraries
- `sequencer`: Executable binary

---

## Phase 4: Development Workflow

### Step 4.1: Making Your First Change

**Example: Add a debug print to understand program flow**

1. **Open a source file:**
```bash
# Use your favorite editor (vim, nano, VS Code, etc.)
open src/apps/sequencer/Main.cpp
```

2. **Add a debug print:**
```cpp
// In src/apps/sequencer/Main.cpp
#include <cstdio>

int main() {
    printf("=== DEBUG: PEW|FORMER Sequencer Starting ===\n");

    // ... existing code ...

    return 0;
}
```

3. **Rebuild (incremental build):**
```bash
cd build/sim/debug
make -j
```

**Expected:** Only changed files recompile (~5-10 seconds)

4. **Run and verify:**
```bash
./src/apps/sequencer/sequencer
```

You should see your debug message in the terminal before the window opens.

### Step 4.2: Iterative Development Cycle

```
┌─────────────────────────────────────┐
│ 1. Edit code (add feature/fix bug)  │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│ 2. Build: make -j                   │
│    (check for compile errors)       │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│ 3. Run: ./src/apps/sequencer/...    │
│    (test manually)                  │
└──────────────┬──────────────────────┘
               ↓
┌─────────────────────────────────────┐
│ 4. Test: make test                  │
│    (automated tests)                │
└──────────────┬──────────────────────┘
               ↓
          Does it work?
          YES → Commit
          NO  → Go to step 1
```

**Pro Tips:**
- Keep a terminal open in `build/sim/debug/` for quick rebuilds
- Use `make -j8` to limit parallel jobs if your Mac gets too hot
- Test frequently (after every small change)

### Step 4.3: Example Feature Addition

**Goal:** Add a new configuration option

```cpp
// In src/apps/sequencer/model/Settings.h

class Settings {
public:
    // Existing settings...

    // Add your new setting
    int myNewSetting() const { return _myNewSetting; }
    void setMyNewSetting(int value) { _myNewSetting = value; }

private:
    int _myNewSetting = 0;  // Default value
};
```

```cpp
// In src/apps/sequencer/ui/pages/SettingsPage.cpp

void SettingsPage::draw() {
    // Add UI for your setting
    drawItem("My Setting", settings.myNewSetting());
}
```

**Build and test:**
```bash
cd build/sim/debug
make -j
./src/apps/sequencer/sequencer
# Navigate to settings page and verify
```

---

## Phase 5: Testing Your Changes

### Step 5.1: Manual Testing (Simulator)

**Interactive testing workflow:**

1. **Start simulator:**
```bash
cd build/sim/debug
./src/apps/sequencer/sequencer
```

2. **Test your feature:**
   - Navigate through UI menus
   - Click buttons, turn encoders
   - Verify expected behavior

3. **Check console output:**
   - Look for errors or warnings
   - Check your debug prints
   - Watch for crashes or freezes

4. **Test edge cases:**
   - Maximum/minimum values
   - Rapid input changes
   - Invalid states

### Step 5.2: Running Unit Tests

**Build and run C++ tests:**

```bash
cd build/sim/release  # Tests typically run in release mode
make -j               # Build everything including tests
make test             # Run all tests
```

**Expected output:**
```
Running tests...
Test project /home/user/performer-ph/build/sim/release
    Start 1: SequencerTests
1/3 Test #1: SequencerTests ...................   Passed    0.12 sec
    Start 2: CoreTests
2/3 Test #2: CoreTests ........................   Passed    0.08 sec
    Start 3: EngineTests
3/3 Test #3: EngineTests ......................   Passed    0.15 sec

100% tests passed, 0 tests failed out of 3
```

**If tests fail:**
```
Test #1: SequencerTests ......................***Failed    0.12 sec

--- Output ---
[FAIL] MyFeature::basic_test
  Expected: 4
  Got: 5
  At: src/tests/unit/sequencer/MyFeatureTest.cpp:15
```

### Step 5.3: Running Python Integration Tests

```bash
# From repository root
python3 src/apps/sequencer/tests/runner.py
```

**What this does:**
- Loads the simulator via Python bindings
- Runs automated UI interaction tests
- Verifies complex workflows (loading projects, saving, etc.)

### Step 5.4: Writing Your Own Unit Test

**Create a new test file:**

`src/tests/unit/sequencer/MyFeatureTest.cpp`:

```cpp
#include "test/UnitTest.h"

// Include the code you're testing
#include "apps/sequencer/model/Settings.h"

UNIT_TEST("MyNewFeature") {

CASE("should handle basic input") {
    Settings settings;
    settings.setMyNewSetting(42);

    expectEqual(settings.myNewSetting(), 42, "setting stores value");
}

CASE("should clamp out of range values") {
    Settings settings;
    settings.setMyNewSetting(-10);

    expectTrue(settings.myNewSetting() >= 0, "value should be clamped");
}

CASE("should have sensible default") {
    Settings settings;

    expectEqual(settings.myNewSetting(), 0, "default is zero");
}

}  // UNIT_TEST
```

**Test assertion macros:**
- `expectEqual(a, b, "message")` - Check equality
- `expectTrue(condition, "message")` - Check boolean
- `expectFalse(condition, "message")` - Check negation
- `expect(condition, "message")` - Generic assertion

**Rebuild and run:**
```bash
cd build/sim/release
make -j
make test
```

### Step 5.5: Test-Driven Development (TDD)

**Recommended workflow:**

1. **Write test first** (it will fail)
2. **Implement feature** (make test pass)
3. **Refactor** (improve code while tests pass)

**Example:**
```bash
# 1. Write test
vim src/tests/unit/sequencer/MyFeatureTest.cpp
cd build/sim/release && make test  # FAIL (expected)

# 2. Implement feature
vim src/apps/sequencer/model/Settings.cpp
cd build/sim/release && make test  # PASS

# 3. Refactor
vim src/apps/sequencer/model/Settings.cpp
cd build/sim/release && make test  # Still PASS
```

---

## Phase 6: Working with STM32 Hardware (Advanced)

### Step 6.1: Setup Hardware Build

```bash
# One-time setup (requires ARM toolchain from Phase 1.3)
make setup_stm32

# This creates: build/stm32/debug and build/stm32/release
```

### Step 6.2: Build for Hardware

```bash
cd build/stm32/release
make -j sequencer

# Output files:
# - sequencer.bin  (binary for flashing)
# - sequencer.elf  (with debug symbols)
# - sequencer.hex  (Intel HEX format)
```

**Build differences:**
- Cross-compiles for ARM Cortex-M4 (not x86_64)
- Uses FreeRTOS (real-time OS)
- No SDL2 (uses actual hardware peripherals)
- Optimized for embedded constraints

### Step 6.3: Flashing to Hardware

**Requirements:**
- JTAG programmer (Olimex ARM-USB-OCD-H recommended)
- STM32F4 hardware
- OpenOCD installed

```bash
# Connect JTAG programmer to Mac USB and hardware board
cd build/stm32/release
make flash_sequencer
```

**Expected output:**
```
Open On-Chip Debugger 0.10.0
...
Info : flash write algorithm found in cache: stm32f4x.flash
Info : Padding image section 0 with 0 bytes
wrote 524288 bytes from file sequencer.bin in 12.345s (41.500 KiB/s)
```

**Troubleshooting flashing on macOS:**

1. **"Can't find JTAG adapter":**
   ```bash
   # Check USB devices
   system_profiler SPUSBDataType | grep -i olimex

   # Install FTDI drivers if needed
   brew install libftdi
   ```

2. **Permission denied:**
   ```bash
   # Run with sudo (not ideal but may work)
   sudo make flash_sequencer
   ```

3. **OpenOCD not working:**
   - Consider using a Linux VM (see `Vagrantfile`)
   - Or use ST-Link utility on another machine

### Step 6.4: Creating UPDATE.DAT for SD Card Firmware Updates

**This is the recommended method for updating your PEW|FORMER hardware!**

SD card updates are much easier than JTAG flashing - you just copy a file to an SD card and the bootloader handles the rest. This is perfect for distributing firmware to users or for quick updates during development.

#### What is UPDATE.DAT?

UPDATE.DAT is a special firmware file that contains:
- Your compiled sequencer binary (sequencer.bin)
- MD5 checksum for verification
- Version information embedded at offset 0x400

The bootloader reads this file from the SD card, verifies its integrity, and flashes it to the device.

#### Building UPDATE.DAT

```bash
# 1. Build the sequencer firmware
cd build/stm32/release
make -j sequencer

# This automatically generates several files:
# - sequencer.elf  (debug symbols)
# - sequencer.bin  (raw binary)
# - sequencer.hex  (Intel HEX format)
# - UPDATE.DAT     (SD card update file) ← This is what you need!
```

**Where to find it:**
```
build/stm32/release/src/apps/sequencer/UPDATE.DAT
```

#### Preparing the SD Card

1. **Format SD card:**
   - File system: **FAT32** (required!)
   - Any size SD card should work

2. **Copy UPDATE.DAT to SD card root:**
   ```bash
   # On macOS
   cp build/stm32/release/src/apps/sequencer/UPDATE.DAT /Volumes/YOUR_SD_CARD/

   # Verify it's in the root directory
   ls /Volumes/YOUR_SD_CARD/
   # Should show: UPDATE.DAT
   ```

3. **Eject SD card safely:**
   ```bash
   diskutil eject /Volumes/YOUR_SD_CARD
   ```

#### Installing Firmware via SD Card

There are **two ways** to enter the bootloader and perform the update:

**Method 1: Press encoder during power-on (Recommended)**
1. Power off your PEW|FORMER sequencer
2. Insert SD card with UPDATE.DAT into the SD card slot
3. **Hold down the encoder button**
4. Power on the device (while still holding encoder)
5. Release encoder when bootloader screen appears

**Method 2: From the user interface**
1. Insert SD card with UPDATE.DAT
2. Navigate to: **System → Update**
3. Follow on-screen instructions

#### Update Process Flow

Once in bootloader mode:

1. **Detection:**
   - Bootloader mounts SD card
   - Looks for UPDATE.DAT in root directory
   - Reads version information from the file

2. **Display current vs. update version:**
   ```
   Current: v0.4.0
   Update:  v0.5.0
   ```

3. **MD5 Verification:**
   - Computes checksum of UPDATE.DAT
   - Compares with embedded MD5
   - Shows progress bar during verification

4. **User Confirmation:**
   ```
   write update?
   no / YES
   ```
   - Turn encoder to select YES or no
   - Press encoder to confirm

5. **Flashing:**
   - Erases flash sectors
   - Writes new firmware (shows progress)
   - Verifies written data with MD5

6. **Completion:**
   - If successful: Device reboots into new firmware
   - If failed: Shows error, stays in bootloader

#### Important Notes

**First-time setup:**
- The bootloader itself must be flashed via JTAG first (one-time setup)
- After bootloader is installed, all future updates can use SD card method

**File requirements:**
- Filename must be exactly `UPDATE.DAT` (case-sensitive)
- Must be in root directory (not in a subfolder)
- SD card must be FAT32 formatted

**Version information:**
The version displayed comes from `src/apps/sequencer/Config.h`:
```cpp
#define CONFIG_VERSION_NAME     "PEW|FORMER SEQUENCER"
#define CONFIG_VERSION_MAJOR    0
#define CONFIG_VERSION_MINOR    5
#define CONFIG_VERSION_REVISION 0
```

To change the version for your build, edit these values before compiling.

#### Troubleshooting SD Card Updates

**Problem: Bootloader doesn't detect UPDATE.DAT**
- Check filename is exactly `UPDATE.DAT` (not `update.dat` or `UPDATE.DAT.txt`)
- Verify file is in root directory: `/UPDATE.DAT`
- Ensure SD card is FAT32 formatted
- Try a different SD card

**Problem: "MD5 verification failed"**
- File may be corrupted during copy
- Rebuild UPDATE.DAT: `make -j sequencer`
- Copy again to SD card
- Eject SD card properly before removing

**Problem: Update starts but fails during flash**
- Device will stay in bootloader mode (safe state)
- Try the update again
- Check that UPDATE.DAT is valid
- May need to JTAG flash if bootloader is corrupted

**Problem: Bootloader won't enter with encoder press**
- Make sure encoder is pressed BEFORE powering on
- Keep holding until bootloader screen appears
- Try Method 2 (from UI) instead
- Check that bootloader is installed (may need JTAG flash first)

#### Developer Workflow: Quick Updates

For rapid development iteration:

```bash
# 1. Make code changes
vim src/apps/sequencer/engine/Engine.cpp

# 2. Build new firmware
cd build/stm32/release
make -j sequencer

# 3. Copy to SD card (keep it mounted)
cp src/apps/sequencer/UPDATE.DAT /Volumes/SD_CARD/

# 4. Eject and install on hardware
diskutil eject /Volumes/SD_CARD

# 5. Power on device with encoder pressed
# 6. Confirm update
# 7. Test new firmware
# 8. Repeat!
```

**Pro tip:** Keep an SD card dedicated for firmware updates so you don't need to reformat each time.

#### Distributing Your Firmware

To share your custom firmware build:

1. Build release version:
   ```bash
   cd build/stm32/release
   make -j sequencer
   ```

2. Copy UPDATE.DAT with a descriptive name:
   ```bash
   cp src/apps/sequencer/UPDATE.DAT ~/Downloads/performer-custom-v0.5.0-UPDATE.DAT
   ```

3. Share with instructions:
   - Rename file to `UPDATE.DAT` before copying to SD card
   - Follow update procedure above

### Step 6.5: Hardware Debugging

**Serial debugging (UART):**
```cpp
// In your code
#include "os/os.h"

void myFunction() {
    OS::Debug::printf("Hardware debug: value=%d\n", myValue);
}
```

**View output:**
```bash
# Connect USB-serial adapter
screen /dev/tty.usbserial-XXXXXXXX 115200
```

**JTAG debugging:**
- Requires OpenOCD + GDB setup
- See `.vscode/launch.json` for VS Code debugging config
- Advanced topic - consult STM32 documentation

---

## Phase 7: Debugging Tips for Beginners

### Common Build Errors

#### Error: "SDL2 not found"

```
CMake Error: Could not find SDL2
```

**Fix:**
```bash
brew install sdl2
rm -rf build
make setup_sim  # Reconfigure
```

#### Error: "No such file or directory" (missing headers)

```
fatal error: 'libopencm3/stm32/rcc.h' file not found
```

**Fix:**
```bash
# Forgot to initialize git submodules
git submodule update --init --recursive
```

#### Error: "undefined reference" (linker error)

```
undefined reference to `MyClass::myMethod()`
```

**Fix:**
- Make sure implementation exists in `.cpp` file
- Check that CMakeLists.txt includes the source file
- Verify function signature matches declaration

#### Error: "make: command not found"

```bash
# Install Xcode command line tools
xcode-select --install
```

### Debugging C++ Code

#### Print Debugging (easiest for beginners)

```cpp
#include <cstdio>

void myFunction(int value) {
    printf("DEBUG: Entering myFunction with value=%d\n", value);

    // Your code...

    printf("DEBUG: Exiting myFunction\n");
}
```

**Better debugging macros:**
```cpp
// In your code
#define DEBUG_PRINT(fmt, ...) \
    printf("[DEBUG %s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

DEBUG_PRINT("Variable x=%d, y=%d", x, y);
// Output: [DEBUG MyFile.cpp:42] Variable x=10, y=20
```

#### Using VS Code Debugger

1. **Install VS Code extensions:**
   - C/C++ (Microsoft)
   - CMake Tools

2. **Open project:**
   ```bash
   code .  # From repository root
   ```

3. **Set breakpoints:**
   - Click in left margin (red dot appears)

4. **Start debugging:**
   - Press `F5` or click "Run and Debug"
   - Program stops at breakpoints
   - Inspect variables, step through code

5. **Debugging controls:**
   - `F10`: Step over (next line)
   - `F11`: Step into (enter function)
   - `F5`: Continue
   - `Shift+F5`: Stop

#### GDB Command Line (advanced)

```bash
cd build/sim/debug
gdb ./src/apps/sequencer/sequencer

(gdb) break Main.cpp:42    # Set breakpoint
(gdb) run                   # Start program
(gdb) print myVariable      # Inspect variable
(gdb) step                  # Step through code
(gdb) continue              # Continue execution
```

### Understanding Compiler Errors

**Anatomy of a compiler error:**

```
src/apps/sequencer/engine/Engine.cpp:42:5: error: 'trackCount' was not declared in this scope
   42 |     trackCount = 8;
      |     ^~~~~~~~~~
```

- **File:** `src/apps/sequencer/engine/Engine.cpp`
- **Line:** 42
- **Column:** 5
- **Error type:** Undeclared variable
- **Symbol:** `trackCount`
- **Code context:** Shows the problematic line

**Common error patterns:**

1. **Undeclared identifier:**
   ```
   error: 'myVar' was not declared in this scope
   ```
   **Fix:** Declare variable, add `#include`, or check spelling

2. **Type mismatch:**
   ```
   error: cannot convert 'int' to 'bool'
   ```
   **Fix:** Use correct type or cast: `static_cast<bool>(value)`

3. **Missing semicolon:**
   ```
   error: expected ';' before 'myFunction'
   ```
   **Fix:** Add semicolon to previous line

4. **Undefined reference:**
   ```
   undefined reference to `MyClass::method()`
   ```
   **Fix:** Implement the method or link the library

### Debugging Crashes

**Segmentation fault (simulator):**
```bash
# Run with debugger to find crash location
gdb ./src/apps/sequencer/sequencer
(gdb) run
# Program crashes
(gdb) backtrace  # Shows call stack at crash
```

**Common crash causes:**
- Null pointer dereference: `ptr->method()` when `ptr == nullptr`
- Array out of bounds: `array[999]` when array size is 100
- Stack overflow: Infinite recursion

---

## Phase 8: Best Practices

### Code Organization

1. **Keep changes small and focused**
   - One feature or bug fix per commit
   - Easier to review and debug

2. **Test frequently**
   - Build after every small change
   - Catch errors early

3. **Use the simulator first**
   - 10x faster than hardware flashing
   - Easier debugging with printf/gdb

4. **Read existing code**
   - Learn patterns used in the project
   - Match coding style

### Coding Style

**Follow project conventions:**

```cpp
// Class names: PascalCase
class SequenceEngine {
public:
    // Method names: camelCase
    void playSequence();

    // Member variables: _camelCase with underscore
    int _trackCount;

    // Constants: kPascalCase
    static const int kMaxTracks = 8;
};

// Functions: camelCase
void processStep(int stepIndex) {
    // Local variables: camelCase
    int noteValue = 60;
}
```

**Comments:**
```cpp
// Good: Explain WHY, not WHAT
// Clamp to valid MIDI range to prevent overflow in synthesizer
value = std::clamp(value, 0, 127);

// Bad: Obvious comment
// Set value to 60
value = 60;
```

### Git Workflow

**Creating a feature branch:**
```bash
# Always branch from latest main
git checkout main
git pull

# Create feature branch (use descriptive name)
git checkout -b feature/add-swing-quantization
```

**Making commits:**
```bash
# Stage specific files
git add src/apps/sequencer/engine/Engine.cpp
git add src/apps/sequencer/engine/Engine.h

# Commit with descriptive message
git commit -m "Add swing quantization to step sequencer

- Implement swing algorithm in Engine class
- Add swing amount parameter (0-100%)
- Update UI to show swing control
- Add unit tests for swing timing"

# Push to remote
git push -u origin feature/add-swing-quantization
```

**Commit message best practices:**
- First line: Short summary (50 chars max)
- Blank line
- Detailed description if needed
- Reference issue numbers: "Fixes #123"

### Documentation

**Document your code:**

```cpp
/**
 * Applies swing quantization to step timing
 *
 * @param stepIndex The step to quantize (0-15)
 * @param swingAmount Swing intensity (0-100, where 0=none, 100=maximum)
 * @return Adjusted timing in milliseconds
 */
int applySwing(int stepIndex, int swingAmount) {
    // Implementation...
}
```

**Update documentation:**
- Add to relevant `.md` files in `doc/`
- Update README if adding major features
- Document configuration options

### Performance Considerations

**Embedded systems have constraints:**

```cpp
// GOOD: Stack allocation (fast)
int buffer[128];

// BAD: Heap allocation (slow, fragmentation)
int* buffer = new int[128];  // Avoid on embedded!

// GOOD: Const references (no copy)
void processTrack(const Track& track);

// BAD: Pass by value (copies data)
void processTrack(Track track);
```

**Optimize hot paths:**
- Profile before optimizing
- Audio/sequencer engines are performance-critical
- UI code is less critical

---

## Quick Reference Commands

### Daily Development

```bash
# Build simulator (debug)
cd build/sim/debug && make -j

# Run simulator
./src/apps/sequencer/sequencer

# Build and test (release)
cd build/sim/release && make -j && make test

# Clean build (when things get weird)
rm -rf build && make setup_sim

# Search for symbol in code
grep -r "myFunction" src/

# Find files by name
find src/ -name "*Engine*"
```

### Git Commands

```bash
# Check status
git status

# View changes
git diff

# Commit changes
git add .
git commit -m "Your message"

# Push to remote
git push

# Pull latest changes
git pull --rebase

# View commit history
git log --oneline --graph
```

### CMake Commands

```bash
# Reconfigure build
cd build/sim/debug
cmake ../..

# Clean and reconfigure
rm -rf build/sim/debug
mkdir -p build/sim/debug
cd build/sim/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../..
```

### Debugging Commands

```bash
# Run with GDB
gdb ./src/apps/sequencer/sequencer
(gdb) run
(gdb) backtrace
(gdb) quit

# Check for memory leaks (if valgrind installed)
valgrind ./src/apps/sequencer/sequencer

# Monitor resource usage
top -pid $(pgrep sequencer)
```

---

## Learning Resources

### C++ Learning
- **Learn C++:** https://www.learncpp.com/ (excellent beginner resource)
- **C++ Reference:** https://en.cppreference.com/
- **Project examples:** Read code in `src/apps/sequencer/`

### Embedded Systems
- **STM32 Reference Manual:** ST Microelectronics website
- **FreeRTOS Documentation:** https://www.freertos.org/
- **libopencm3 Examples:** Check `libs/libopencm3/` submodule

### Tools
- **CMake Tutorial:** https://cmake.org/cmake/help/latest/guide/tutorial/
- **GDB Cheat Sheet:** https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf
- **Git Book:** https://git-scm.com/book/en/v2

### Project-Specific
- **Design Document:** `doc/DesignDocument.md` - Hardware specs and features
- **Existing Tests:** `src/tests/unit/` - Code examples and patterns
- **Original Project:** https://westlicht.github.io/performer

---

## Troubleshooting

### Build Issues

| Problem | Solution |
|---------|----------|
| SDL2 not found | `brew install sdl2`, then `rm -rf build && make setup_sim` |
| Submodules missing | `git submodule update --init --recursive` |
| CMake too old | `brew upgrade cmake` |
| Compile errors after `git pull` | `rm -rf build && make setup_sim && make -j` |
| Linker errors | Check CMakeLists.txt includes all source files |

### Runtime Issues

| Problem | Solution |
|---------|----------|
| Simulator crashes on start | Run with `gdb` to get backtrace |
| Display not rendering | Check OpenGL drivers, try different macOS version |
| No MIDI devices | Check System Preferences → Security & Privacy |
| Audio not working | Check Audio MIDI Setup utility |

### Hardware Issues

| Problem | Solution |
|---------|----------|
| Can't flash firmware | Check JTAG connections, try `sudo`, use Linux VM |
| Hardware not responding | Check power, verify flash succeeded |
| Serial output not visible | Check baud rate (115200), USB-serial adapter |

---

## Next Steps

### Beginner Path (Week 1-2)

- [ ] Complete Phase 1: Setup development environment
- [ ] Build and run simulator (Phase 3)
- [ ] Make a trivial code change and rebuild
- [ ] Run unit tests successfully
- [ ] Read through `doc/DesignDocument.md`
- [ ] Explore sequencer UI in simulator
- [ ] Understand code in `src/apps/sequencer/Main.cpp`

### Intermediate Path (Week 3-4)

- [ ] Implement a small feature (add a setting)
- [ ] Write unit tests for your feature
- [ ] Read engine code: `src/apps/sequencer/engine/`
- [ ] Understand model classes: `src/apps/sequencer/model/`
- [ ] Debug a simple issue with GDB
- [ ] Create feature branch and commit changes

### Advanced Path (Month 2+)

- [ ] Setup hardware build environment
- [ ] Flash firmware to real hardware
- [ ] Implement complex sequencer feature
- [ ] Profile and optimize performance
- [ ] Contribute back to the project
- [ ] Read FreeRTOS and STM32 documentation

---

## Getting Help

### Resources

- **Project README:** `README.md`
- **This guide:** `doc/MacOSDevelopmentGuide.md`
- **Design docs:** `doc/DesignDocument.md`
- **Source code:** Comments and examples throughout `src/`

### Community

- **Original project:** https://github.com/westlicht/performer
- **Hardware discussions:** Check original project issues
- **Fork repository:** https://github.com/onoma2/performer-ph

### Debugging Strategy

1. **Read the error message carefully**
2. **Search for the error online** (Stack Overflow, etc.)
3. **Check similar code** in the project
4. **Simplify** - Make smallest reproducible example
5. **Add debug prints** to understand flow
6. **Use GDB** to inspect state
7. **Ask for help** with specific details

---

## Appendix: Keyboard Shortcuts (Simulator)

| Key | Action |
|-----|--------|
| ESC | Exit simulator |
| Mouse | Click buttons, drag encoders |
| Arrow keys | Navigate menus (possibly) |
| F1-F12 | Function keys (check UI) |

*Note: Exact mappings may vary - check source code in `src/platform/sim/` for details*

---

**Happy coding! 🎛️🎶**

Remember: Start small, test often, and use the simulator for rapid iteration. The best way to learn embedded C++ is by doing!
