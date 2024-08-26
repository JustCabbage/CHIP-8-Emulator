# CHIP-8 Emulator [![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://lbesson.mit-license.org/)

## What is this
This is a relatively finished implementation of the [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) virtual machine written in C++20 and [SFML](https://www.sfml-dev.org/). 

**This emulator currently does not have any sound output at the moment. This feature will be added at some point in the future.**

## Building

To build this project you must have [CMake](https://cmake.org/) installed. This project should also clone all relevant dependencies for you.

```bash
git clone https://github.com/JustCabbage/CHIP-8-Emulator --recursive
cd CHIP-8-Emulator && mkdir build && cd build
cmake .. && cmake --build .
```

## Controls
```
CHIP-8                  Keyboard
1    2    3    C        1    2    3    4
4    5    6    D        Q    W    E    R
7    8    9    E        A    S    D    F
A    0    B    F        Z    X    C    V
```

## Screenshots

![Home](https://raw.github.com/JustCabbage/CHIP-8-Emulator/master/Screenshots/home.png)
![Menu](https://raw.github.com/JustCabbage/CHIP-8-Emulator/master/Screenshots/menu.png)
![Flags](https://raw.github.com/JustCabbage/CHIP-8-Emulator/master/Screenshots/flags.png)
![Keypad](https://raw.github.com/JustCabbage/CHIP-8-Emulator/master/Screenshots/keypad.png)
![SpaceInvaders](https://raw.github.com/JustCabbage/CHIP-8-Emulator/master/Screenshots/space-invaders.png)

## Credits
 - [CHIP-8 Documentation](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
 - [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite)
 - [SFML](https://www.sfml-dev.org/)
