GASM — Ghost Assembly Language

GASM (Ghost Assembly Language) is a custom stack-based assembly language and virtual machine designed from scratch.

The project implements its own assembly language, assembler, bytecode format, and virtual machine. GASM programs can be assembled into executable bytecode and then executed by the GASM virtual machine.

The project is designed as a small, self-contained computing environment with support for computation, memory management, control flow, functions, file I/O, graphics, keyboard input, timing, and dynamic memory allocation.

«Project status: Experimental / actively evolving»

---

Architecture

GASM is built around three main components:

                 GASM source
                    (.gasm)
                       │
                       ▼
              ┌─────────────────┐
              │     Assembler   │
              │                 │
              │    Pass 1       │
              │    Pass 2       │
              └────────┬────────┘
                       │
                       ▼
                  GASM binary
                       │
                       ▼
              ┌─────────────────┐
              │   GASM Virtual  │
              │     Machine     │
              └────────┬────────┘
                       │
          ┌────────────┼────────────┐
          ▼            ▼            ▼
       Memory       Graphics      Input

The repository contains the assembler and VM implementation as well as example GASM programs.

---

Features

Virtual Machine

The GASM VM provides:

- Stack-based execution
- Arithmetic operations
- Comparison and conditional branching
- Function calls and returns
- Registers through "STORE" / "LOAD"
- Direct memory access
- Dynamic memory allocation
- File I/O
- String handling and formatted output
- Keyboard input
- Random number generation
- Timing support
- Graphics and framebuffer operations
- Fixed-point arithmetic
- Memory copy/set operations

The instruction set currently contains 68 opcodes.

---

Instruction Set

Stack and Arithmetic

Instruction| Description
"PUSH"| Push a value onto the stack
"POP"| Remove the top stack value
"DUP"| Duplicate the top stack value
"DUP2"| Duplicate the top two stack values
"SWAP"| Swap stack values
"ADD"| Addition
"SUB"| Subtraction
"MUL"| Multiplication
"DIV"| Division
"MOD"| Modulo

Variables and Memory

Instruction| Description
"STORE"| Store a value
"LOAD"| Load a value
"PEEK"| Read memory
"POKE"| Write memory
"PEEKL"| Read a larger memory value
"POKEL"| Write a larger memory value
"PEEK8"| Read an 8-bit value
"POKE8"| Write an 8-bit value
"GETSP"| Obtain the stack pointer
"GETBP"| Obtain the base pointer
"MALLOC"| Allocate memory
"FREE"| Free allocated memory
"MEMCPY"| Copy memory
"MEMSET"| Set a memory region

Control Flow

Instruction| Description
"JMP"| Unconditional jump
"CMP"| Compare values
"JE"| Jump if equal
"JNE"| Jump if not equal
"JL"| Jump if less
"JG"| Jump if greater
"JLE"| Jump if less than or equal
"JGE"| Jump if greater than or equal
"FUNC"| Define a function
"CALL"| Call a function
"RET"| Return from a function
"LABEL"| Define a label

Logic and Bit Operations

Instruction| Description
"AND"| Bitwise AND
"OR"| Bitwise OR
"NOT"| Bitwise NOT
"XOR"| Bitwise XOR
"SHL"| Shift left
"SHR"| Shift right

Input / Output

Instruction| Description
"PRINT"| Print a value
"PRINT_C"| Print a character
"PRINT_STR"| Print a string
"READ_INT"| Read an integer
"KEY"| Read keyboard state

File I/O

Instruction| Description
"FOPEN"| Open a file
"FCLOSE"| Close a file
"FREAD"| Read from a file
"FWRITE"| Write to a file
"FSEEK"| Seek within a file
"FTELL"| Get file position
"FSIZE"| Get file size
"FEOF"| Check end-of-file

Graphics and Timing

Instruction| Description
"DRAW"| Draw/update graphics
"CLS"| Clear the screen
"SHOW"| Display the framebuffer
"BLIT"| Copy graphical data
"FILLRECT"| Fill a rectangle
"GETBB"| Access the back buffer
"SLEEP"| Sleep for a period
"TICKS"| Obtain timing information
"RAND"| Generate a random value

Other

Instruction| Description
"HALT"| Stop execution
"INCLUDE"| Include another GASM source file
"MUL_FIXED"| Fixed-point multiplication
"DIV_FIXED"| Fixed-point division

The opcode definitions and instruction metadata are centralized in "gasm.h".

---

Memory Architecture

The VM exposes a 16 MB virtual address space divided into several regions.

Region| Size| Purpose
Code| 1 MB| Program instructions
Data| 1 MB| Global data and strings
Heap| 10 MB| Dynamically allocated memory
Stack| 2 MB| Stack, local data and return information
VRAM| 320 × 240| Framebuffer
Input| 256 bytes| Keyboard/input state

The stack grows downward.

The VM uses a "320 × 240" framebuffer for graphical output.

---

Assembler

GASM includes a custom assembler written in C.

The assembler uses a two-pass design.

Pass 1

The first pass analyzes the source program and determines:

- Instruction sizes
- Labels
- Function addresses
- Global variable locations
- Included source files
- Data/string information

Symbol information is collected so that labels and functions can later be resolved to addresses.

Pass 2

The second pass converts the GASM source into binary instructions.

It resolves:

- Opcodes
- Immediate operands
- Character constants
- String addresses
- Global variable addresses
- Function addresses
- Jump targets

The resulting instructions are written to the output binary.

---

Source Files

The repository is organized around the following components:

File| Purpose
"gasm.h"| VM memory layout and opcode definitions
"vm.c"| GASM virtual machine
"assembler.c"| GASM assembler
"translator.c"| Translation/runtime support
"gasm.c"| Main GASM entry point
"*.gasm"| Example GASM programs
"*.bin"| Example assembled programs

---

Example GASM Program

A simple GASM program can operate directly on the VM stack:

PUSH 10
PUSH 20
ADD
PRINT
HALT

Conceptually:

10
 │
 ▼
PUSH
 │
20
 │
 ▼
PUSH
 │
 ▼
 ADD
 │
 ▼
30
 │
 ▼
PRINT
 │
 ▼
30

Because GASM is stack-based, arithmetic operations consume their operands from the stack and place their result back onto it.

---

Functions

GASM provides explicit function support through "FUNC", "CALL", and "RET".

A program can therefore organize code into reusable functions instead of relying exclusively on linear execution.

Labels are also supported for control-flow targets, allowing instructions such as "JMP", "JE", "JNE", "JL", "JG", "JLE", and "JGE" to target symbolic locations.

---

Graphics

The VM contains a framebuffer-based graphics subsystem.

The virtual machine exposes a "320 × 240" display and provides instructions for drawing, clearing, displaying, manipulating the back buffer, and filling rectangular regions.

Example graphical programs are included in the repository:

graphics1.gasm
graphics2.gasm
fps.gasm

These programs demonstrate that GASM can be used for more than simple arithmetic and console programs.

---

File I/O

GASM exposes file operations directly through VM instructions.

Programs can:

- Open files
- Read data
- Write data
- Seek to positions
- Query file size
- Query the current position
- Detect end-of-file
- Close files

This allows GASM programs to interact with files without requiring the assembly language itself to implement the underlying operating-system file APIs.

---

Dynamic Memory

The VM provides dynamic allocation through:

MALLOC
FREE

Memory utility operations are also available:

MEMCPY
MEMSET

The VM reserves a 10 MB heap region for dynamically allocated memory.

---

Includes

GASM source files can include other GASM files using:

INCLUDE filename.gasm

The assembler recursively processes included files while avoiding processing the same included library multiple times.

This allows GASM programs to be split into reusable source files rather than keeping everything in a single assembly file.

---

Building

The project is written in C.

A typical build consists of compiling the GASM entry point together with the assembler and VM:

gcc gasm.c assembler.c translator.c vm.c -o gasm

«The graphics/input implementation currently relies on platform-specific functionality, so the project is primarily oriented toward Windows at its current stage.»

---

Running

Once built, the GASM executable can be used to process a GASM program.

For example:

gasm program.gasm

The exact command-line behavior is defined by the current "gasm.c" entry point.

---

Example Programs

The repository contains several example programs:

test.gasm
graphics1.gasm
graphics2.gasm
fps.gasm

These provide examples of using the language and VM for testing, graphics, and interactive programs.

---

Relationship to the C-GASM Compiler

GASM is also the target architecture for a separate project:

C-GASM Compiler

The compiler translates a subset of C into GASM assembly.

The complete pipeline is therefore:

        C Program
            │
            ▼
     C-GASM Compiler
            │
            ▼
        GASM source
            │
            ▼
       GASM Assembler
            │
            ▼
       GASM Bytecode
            │
            ▼
         GASM VM

The two repositories form parts of the same larger project: the compiler provides a higher-level language interface while GASM provides the target assembly language and execution environment.

---

Design Goals

The project was built to explore the implementation of a custom computing environment from the ground up, including:

- Designing an instruction set
- Implementing a stack-based execution model
- Building an assembler
- Implementing a virtual machine
- Designing a virtual memory layout
- Implementing control flow and function calls
- Providing memory management
- Providing I/O primitives
- Adding graphics and input support
- Creating a target architecture for a compiler

Rather than targeting an existing ISA, GASM defines its own instruction set and execution model.

---

Limitations

GASM is an experimental project and is not intended to replace established assembly languages or production virtual machines.

Current limitations include:

- Platform-specific graphics/input implementation
- Fixed virtual memory layout
- Fixed framebuffer resolution
- Experimental instruction-set design
- Limited tooling compared with mature virtual machines
- No formal language specification yet

---

Future Work

Possible future improvements include:

- Formalizing the GASM instruction-set specification
- Improving portability
- Adding better debugging tools
- Adding a disassembler
- Improving error reporting
- Expanding the standard library/runtime
- Adding more graphical primitives
- Improving VM performance
- Expanding compiler support through the C-GASM compiler
- Running larger programs on the VM

---

Project Structure

GASM-Ghost-Assembly-Language/
│
├── assembler.c
├── translator.c
├── vm.c
├── gasm.c
├── gasm.h
│
├── test.gasm
├── graphics1.gasm
├── graphics2.gasm
├── fps.gasm
│
└── fps.bin

---

Related Project

C-GASM Compiler

A custom C compiler targeting GASM.

https://github.com/Pnatehc13/C-GASM-Compiler

---

License

See the repository for the current licensing information.
