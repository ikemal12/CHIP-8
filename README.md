# CHIP-8 Emulator

https://github.com/user-attachments/assets/3a055dac-595a-4d45-a0a8-faed9fdaf30f

## Overview

This is a fairly complete implementation of the CHIP-8 virtual machine, however the system buzzer is currently not functional. It is in a playable state and can run most ROMs such as Tetris shown above.

Summary of architecture:

* 16 8-bit Registers
* 4k Bytes of Memory
* 16-bit Index Register 
* 16-bit Program Counter 
* 16-level Stack
* 8-bit Stack Pointer
* 8-bit Delay Timer
* 8-bit Sound Timer
* 16 Input Keys
* 64x32 Monochrome Display Memory

## Installation & Usage

### 1. Clone the repo

```
git clone https://github.com/yourusername/chip8-emulator.git
cd chip8-emulator
```

### 2. Install dependencies (MSYS2/MinGW)

If you don't have MSYS2, [download and install it](https://www.msys2.org/).

Open the MSYS2 MinGW 64-bit terminal and run:

```
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2
```

### 3. Build and Run

From the project root directory, build the emulator:

```
g++ -std=c++17 -IC:/msys64/mingw64/include/SDL2 -LC:/msys64/mingw64/lib src/*.cpp -lSDL2main -lSDL2 -o chip8_emulator.exe
```

> **Note:** Adjust the include/library paths if your MSYS2 is installed elsewhere.

To run it you will need a ROM file (e.g. [test_opcode.ch8](https://github.com/corax89/chip8-test-rom)). You can find a bunch of other ROMs to test [here](https://github.com/kripod/chip8-roms).

The emulator expects three arguments:

`<Scale> <Delay> <ROM>`

Example:

```
./chip8_emulator.exe 10 2 roms/test_opcode.ch8
```

* `10` = window scale (each CHIP-8 pixel will be 10x10)
* `2` = emulation cycle delay in ms (try 2–10 for most ROMs)
* `roms/test_opcode.ch8` = path to your ROM file

## Notes

* Place your ROM files in a `roms/` folder or specify the path.
* If you get SDL errors, make sure you are running from the MSYS2 MinGW 64-bit terminal.
* For Linux or macOS, install SDL2 and use a similar `g++` command (adjust include/library paths as needed).

## Controls

**CHIP-8 Keypad Layout:**

|     |     |     |     |
|:---:|:---:|:---:|:---:|
|  1  |  2  |  3  |  C  |
|  4  |  5  |  6  |  D  |
|  7  |  8  |  9  |  E  |
|  A  |  0  |  B  |  F  |

**Mapped to your keyboard:**

|     |     |     |     |
|:---:|:---:|:---:|:---:|
|  1  |  2  |  3  |  4  |
|  Q  |  W  |  E  |  R  |
|  A  |  S  |  D  |  F  |
|  Z  |  X  |  C  |  V  |

## References 

Here are some of the resources I used:

* https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
* https://github.com/mattmikolay/chip-8/wiki/CHIP‐8-Instruction-Set

## License

This project is licensed under the [MIT License](LICENSE).

