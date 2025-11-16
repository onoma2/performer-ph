# GEMINI.md - Performer Firmware

## Project Overview

This repository contains the firmware for the **PER|FORMER**, a Eurorack sequencer module. The firmware is written in C++ and can be compiled for both the STM32-based hardware and a simulator for development purposes.

The project uses `CMake` as a build system and `make` for executing build commands. The source code is structured into applications, a core library, and platform-specific code for the hardware and the simulator.

The main application is the `sequencer`, which contains the core logic for the sequencer, including the engine, data model, and user interface.

## Building and Running

### Prerequisites

- **macOS/Linux:** `cmake`, `libtool`, `autoconf`, `libusb-1.0.0-dev`, `libftdi-dev`, `pkg-config`
- **Windows:** Vagrant

### Setup

1.  **Clone the repository:**
    ```bash
    git clone --recursive https://github.com/westlicht/performer.git
    cd performer
    ```

2.  **Install tools (for hardware development):**
    ```bash
    make tools_install
    ```

3.  **Setup build directories:**
    -   **For hardware (STM32):**
        ```bash
        make setup_stm32
        ```
    -   **For simulator:**
        ```bash
        make setup_sim
        ```

### Building

-   **For hardware (STM32):**
    ```bash
    cd build/stm32/release
    make -j sequencer
    ```

-   **For simulator:**
    ```bash
    cd build/sim/debug
    make -j
    ```

### Running the Simulator

```bash
cd build/sim/debug
./src/apps/sequencer/sequencer
```

### Flashing the Firmware

To flash the firmware to the hardware, use the following commands from the `build/stm32/release` directory:

```bash
make flash_bootloader
make flash_sequencer
```

## Development Conventions

The codebase is well-structured, with a clear separation between the core logic, platform-specific code, and applications.

-   **Core Logic:** The `src/core` directory contains the core library, which is shared between the sequencer and other applications.
-   **Platform Abstraction:** The `src/platform` directory contains the platform-specific code for the `stm32` hardware and the `sim` simulator. This allows the core logic to be platform-independent.
-   **Applications:** The `src/apps` directory contains the different applications, such as the `sequencer`, `bootloader`, and `tester`.
-   **User Interface:** The `src/apps/sequencer/ui` directory contains the user interface code, which is separated from the core sequencer logic.
-   **Testing:** The `src/tests` directory contains unit and integration tests. The simulator is also used for testing and debugging.
