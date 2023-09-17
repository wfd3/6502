# 6502
This is a simple 6502 CPU emulator and an example implementation of an Apple 1, which is just simple enough to prove out the emulator.

The 6502 emulator contains a built in debugger and disassembler, supporting breakpoints and memory watchpoints.  The Apple 1 emulator runs original WozMon roms, Apple INTEGER Basic and a backport of Applesoft Basic to the Apple 1.  The roms can be found in `binfiles/`.

The build system is CMake, but there's a top-level Makefile for convenience.  Binaries for the Apple 1 emulator, as well as the two Google Test test programs, will be deposited into `build/bin`

