# 8bit-Numera: A Calculator for TrimUI Devices

## Description
8bit-Numera is a lightweight, no-nonsense calculator application specifically designed for the TrimUI Brick and other compatible retro handheld gaming devices. It provides essential arithmetic functionalities in a simple, easy-to-use interface, optimized for the unique hardware constraints of these portable systems.

## Features
*   Basic arithmetic operations (addition, subtraction, multiplication, division)
*   Optimized for TrimUI Brick and similar retro handhelds
*   Simple and intuitive user interface

## Prerequisites
To build 8bit-Numera from source, you will need:
*   **CMake**: Version 3.10 or higher.
*   **Aarch64 Cross-Compilation Toolchain**: Specifically, the `aarch64-linux-gnu-7.5.0-linaro` toolchain is used for targeting the TrimUI devices. This toolchain can be downloaded from [Linaro Releases](https://releases.linaro.org/components/toolchain/binaries/latest-7/aarch64-linux-gnu/). It should be installed and accessible on your system, typically under `/opt/aarch64-linux-gnu-7.5.0-linaro/`.
    *   An alternative or related toolchain for TrimUI Smart Pro can be found at: [https://github.com/s0ckz/trimui-smart-pro-toolchain.git](https://github.com/s0ckz/trimui-smart-pro-toolchain.git)
*   **SDL2 Development Libraries**: The project relies on SDL2 for graphics and input. The SDL2 development libraries for `aarch64` must be available within your sysroot.

## Building from Source

Follow these steps to compile 8bit-Numera for your TrimUI device:

1.  **Navigate to the project root:**
    ```bash
    cd /path/to/trimui-docker-toolchain/workspace/Trimui-8bit-Numera
    ```

2.  **Create a build directory and navigate into it:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project with CMake, specifying the TrimUI toolchain:**
    Ensure the `toolchain-trimui.cmake` file is accessible from your build environment. The path below assumes you are in the `workspace/Trimui-8bit-Numera/build` directory and `toolchain-trimui.cmake` is in the root of the `trimui-docker-toolchain` directory.
    ```bash
    cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain-trimui.cmake ..
    ```

4.  **Compile the project:**
    ```bash
    make
    ```

Upon successful compilation, an executable named `calculator` will be generated in the `build` directory.

## Usage
To use 8bit-Numera on your TrimUI device, transfer the `calculator` executable to the appropriate application directory on your device. Consult your TrimUI device's documentation for specific instructions on installing custom applications.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.