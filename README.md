# CHIP-8 Emulator

A simple CHIP-8 emulator written in C++.

## Description

This project is an implementation of a CHIP-8 virtual machine. CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. It was made to allow video games to be more easily programmed for these computers.

## Building and Running

This project uses Meson as its build system.

### Prerequisites

- A C++ compiler (supporting C++20)
- Meson
- Ninja

### Building

1.  **Navigate to the project directory:**
    ```bash
    cd path/to/chip8
    ```
2.  **Create a build directory (if it doesn't exist) and configure Meson:**
    ```bash
    meson setup build
    ```
    (Note: Your existing build directory is named `build`. If you prefer a different name like `builddir`, you can use that instead, but be consistent.)
3.  **Compile the project:**
    ```bash
    meson compile -C build
    ```
    Alternatively, you can navigate into the build directory and run `ninja`:
    ```bash
    cd build
    ninja
    ```

### Running

After a successful build, the executable `chip8` will be located in the `build` directory.

```bash
./build/chip8
```
