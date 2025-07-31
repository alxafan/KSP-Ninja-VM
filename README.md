# KSP Exercise Solutions

This repository contains solutions to all exercises (0 through 8) from the **KSP (Concepts of Systems Programming)** course.

The project demonstrates low-level programming techniques and system-level concepts.

My solution is contained in the njvm.c file, the rest was provided in the excercise.
## Toolchain Overview

The project uses a custom compiler and runtime environment:

- **`njc`** — compiles `.nj` source files to `.asm` (assembly)
- **`nja`** — assembles `.asm` files into `.bin` binaries
- **`njvm`** — runs `.bin` binaries in a custom virtual machine

## How to Compile and Run

Each task is located in its own folder, typically containing a `.nj` source file.

To build and run a file named `Main.nj`:

```bash
# Step 1: Compile .nj source to .nja assembly
njc Main.nj Main.asm

# Step 2: Assemble .nja file to .bin binary
nja Main.asm Main.bin

# Step 3: Execute the program with the custom VM
njvm Main.bin
```
## Notes
This project is educational and follows the structure and semantics defined by the KSP course. Each task illustrates core systems programming concepts such as control flow, memory management, and instruction-level execution.

A script for testing all assembly programs located in /prog, using my vm, is available in the file "testingScript"
