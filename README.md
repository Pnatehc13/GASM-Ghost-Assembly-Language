
# GASM — Ghost Assembly Language

GASM (Ghost Assembly Language) is a custom stack-based assembly language and virtual machine built from scratch.

The project implements its own assembly language, assembler, binary format, and virtual machine. GASM programs can be assembled into executable bytecode and then executed by the GASM virtual machine.

The VM provides support for computation, control flow, functions, memory management, file I/O, graphics, keyboard input, timing, and dynamic memory allocation.

> **Status:** Experimental / actively evolving

---

## Architecture

GASM consists of a custom assembly language, assembler, and virtual machine.

```text
                 GASM Source
                    (.gasm)
                       |
                       v
              +-----------------+
              |    Assembler    |
              |                 |
              |    Pass 1       |
              |    Pass 2       |
              +--------+--------+
                       |
                       v
                 GASM Binary
                       |
                       v
              +-----------------+
              |    GASM VM      |
              |                 |
              | Stack Execution |
              +--------+--------+
                       |
          +------------+------------+
          |            |            |
          v            v            v
       Memory       Graphics      Input

The overall execution pipeline is:

GASM Source
     |
     v
  Assembler
     |
     v
 GASM Binary
     |
     v
  GASM VM
     |
     v
 Program Execution


---

Features

Virtual Machine

The GASM VM provides:

Stack-based execution

Arithmetic operations

Comparison and conditional branching

Function calls and returns

Registers through STORE / LOAD

Direct memory access

Dynamic memory allocation

File I/O

String handling

Formatted output

Keyboard input

Random number generation

Timing support

Graphics and framebuffer operations

Fixed-point arithmetic

Memory copy/set operations


The current instruction set contains 68 opcodes.


---

Instruction Set

GASM provides instructions for computation, memory management, control flow, I/O, graphics, and system interaction.

Stack and Arithmetic

Instruction	Description

PUSH	Push a value onto the stack
POP	Remove the top stack value
DUP	Duplicate the top stack value
DUP2	Duplicate the top two stack values
SWAP	Swap stack values
ADD	Addition
SUB	Subtraction
MUL	Multiplication
DIV	Division
MOD	Modulo


Variables and Memory

Instruction	Description

STORE	Store a value
LOAD	Load a value
PEEK	Read memory
POKE	Write memory
PEEKL	Read a larger memory value
POKEL	Write a larger memory value
PEEK8	Read an 8-bit value
POKE8	Write an 8-bit value
GETSP	Obtain the stack pointer
GETBP	Obtain the base pointer
MALLOC	Allocate memory
FREE	Free allocated memory
MEMCPY	Copy memory
MEMSET	Set a memory region


Control Flow

Instruction	Description

JMP	Unconditional jump
CMP	Compare values
JE	Jump if equal
JNE	Jump if not equal
JL	Jump if less
JG	Jump if greater
JLE	Jump if less than or equal
JGE	Jump if greater than or equal
FUNC	Define a function
CALL	Call a function
RET	Return from a function
LABEL	Define a label


Logic and Bit Operations

Instruction	Description

AND	Bitwise AND
OR	Bitwise OR
NOT	Bitwise NOT
XOR	Bitwise XOR
SHL	Shift left
SHR	Shift right


Input / Output

Instruction	Description

PRINT	Print a value
PRINT_C	Print a character
PRINT_STR	Print a string
READ_INT	Read an integer
KEY	Read keyboard state


File I/O

Instruction	Description

FOPEN	Open a file
FCLOSE	Close a file
FREAD	Read from a file
FWRITE	Write to a file
FSEEK	Seek within a file
FTELL	Get the current file position
FSIZE	Get the file size
FEOF	Check for end-of-file


Graphics and Timing

Instruction	Description

DRAW	Draw/update graphics
CLS	Clear the screen
SHOW	Display the framebuffer
BLIT	Copy graphical data
FILLRECT	Fill a rectangle
GETBB	Access the back buffer
SLEEP	Sleep for a period
TICKS	Obtain timing information
RAND	Generate a random value


Other

Instruction	Description

HALT	Stop execution
INCLUDE	Include another GASM source file
MUL_FIXED	Fixed-point multiplication
DIV_FIXED	Fixed-point division



---

Memory Architecture

The GASM VM exposes a 16 MB virtual address space divided into multiple regions.

Region	Size	Purpose

Code	1 MB	Program instructions
Data	1 MB	Global data and strings
Heap	10 MB	Dynamically allocated memory
Stack	2 MB	Stack and runtime data
VRAM	320 × 240	Framebuffer
Input	256 bytes	Keyboard/input state


The VM uses a 320 × 240 framebuffer for graphical output.

The heap is used for dynamic memory allocation through the MALLOC and FREE instructions.


---

Stack-Based Execution

GASM uses a stack-based execution model.

Operands for arithmetic and other operations are placed on the VM stack.

For example:

PUSH 10
PUSH 20
ADD
PRINT
HALT

The execution can be visualized as:

PUSH 10

Stack:
[10]


PUSH 20

Stack:
[10, 20]


ADD

Stack:
[30]


PRINT

Output:
30

This model forms the basis of the VM's execution architecture.


---

Functions

GASM provides explicit support for functions through:

FUNC
CALL
RET

Labels can also be used as targets for control-flow instructions.

For example, conditional branches can target labels using instructions such as:

JMP
JE
JNE
JL
JG
JLE
JGE


---

Assembler

GASM includes a custom assembler written in C.

The assembler uses a two-pass design.

Pass 1

The first pass analyzes the source program and determines information such as:

Instruction locations

Labels

Function addresses

Global variable locations

Included source files

Data and string information


This allows symbolic references to be resolved before binary generation.

Pass 2

The second pass converts the GASM source into binary instructions.

It resolves:

Opcodes

Immediate operands

Character constants

String addresses

Global variable addresses

Function addresses

Jump targets


The resulting binary is then used by the virtual machine.


---

Include System

GASM source files can include other GASM files.

INCLUDE filename.gasm

This allows programs to be separated into reusable source files instead of placing everything into a single assembly file.


---

Dynamic Memory

The VM provides dynamic memory allocation through:

MALLOC
FREE

Memory manipulation is also supported through:

MEMCPY
MEMSET

The VM reserves a 10 MB heap for dynamically allocated memory.


---

File I/O

GASM provides file operations directly through VM instructions.

Programs can:

Open files

Close files

Read data

Write data

Seek to a position

Obtain the current file position

Obtain file size

Check for end-of-file


The relevant instructions are:

FOPEN
FCLOSE
FREAD
FWRITE
FSEEK
FTELL
FSIZE
FEOF


---

Graphics

The GASM VM includes a framebuffer-based graphics system.

The virtual display is:

320 × 240

Graphics-related instructions provide functionality for drawing, clearing, displaying, and manipulating graphical data.

The repository contains example graphical programs demonstrating the graphics subsystem.


---

Input

The VM provides keyboard input support through the KEY instruction.

This allows GASM programs to interact with user input and build interactive applications.


---

Timing

The VM provides timing-related functionality through:

SLEEP
TICKS

These instructions allow programs to implement timing-sensitive behavior and measure elapsed execution time.


---

Example Programs

The repository contains several example GASM programs demonstrating different parts of the system.

Examples include:

test.gasm
graphics1.gasm
graphics2.gasm
fps.gasm

These programs are useful for understanding the language and testing the VM.


---

Repository Structure

GASM-Ghost-Assembly-Language/
|
+-- assembler.c
+-- translator.c
+-- vm.c
+-- gasm.c
+-- gasm.h
|
+-- test.gasm
+-- graphics1.gasm
+-- graphics2.gasm
+-- fps.gasm
|
+-- fps.bin

Main Components

File	Purpose

gasm.h	VM definitions, memory layout and opcode definitions
vm.c	Virtual machine implementation
assembler.c	GASM assembler
translator.c	Translation/runtime support
gasm.c	Main program entry point



---

Building

The project is written in C.

A typical build can be performed using a C compiler such as GCC:

gcc gasm.c assembler.c translator.c vm.c -o gasm

The current graphics and input implementation uses platform-specific functionality, so the project is currently oriented toward Windows.


---

Running

After building the project, a GASM program can be passed to the GASM executable.

For example:

gasm program.gasm

The command-line behavior is implemented by gasm.c.


---

Relationship to the C-GASM Compiler

GASM is also the target architecture for the C-GASM Compiler, a separate project that translates C programs into GASM assembly.

The complete toolchain is:

C Source
                 |
                 v
        +----------------+
        | C-GASM Compiler|
        +-------+--------+
                |
                v
            GASM Source
                |
                v
        +----------------+
        | GASM Assembler |
        +-------+--------+
                |
                v
           GASM Binary
                |
                v
        +----------------+
        |    GASM VM     |
        +-------+--------+
                |
                v
          Program Output

The two repositories therefore form parts of the same larger project:

C-GASM Compiler provides the C-to-GASM compilation layer.

GASM provides the target assembly language, assembler, binary format, and execution environment.


C-GASM Compiler:

https://github.com/Pnatehc13/C-GASM-Compiler


---

Design Goals

The project was created to explore the implementation of a custom computing environment from the ground up.

The main goals include:

Designing a custom instruction set

Implementing a stack-based execution model

Building an assembler

Implementing a virtual machine

Designing a virtual memory layout

Implementing control flow

Supporting functions and function calls

Implementing dynamic memory management

Providing file I/O

Providing graphics and input

Creating a target architecture for a compiler


Instead of targeting an existing ISA, GASM defines its own instruction set and execution model.


---

Limitations

GASM is an experimental project and is not intended to replace production virtual machines or established assembly languages.

Current limitations include:

Platform-specific graphics/input implementation

Fixed virtual memory layout

Fixed framebuffer resolution

Experimental instruction-set design

Limited tooling compared with mature virtual machines

No formal standalone language specification yet



---

Future Work

Possible future improvements include:

Formalizing the GASM instruction-set specification

Improving portability

Adding a debugger

Adding a disassembler

Improving assembler error reporting

Expanding runtime functionality

Adding more graphical primitives

Improving VM performance

Expanding C language support through the C-GASM compiler

Running larger programs on the VM



---

Related Project

C-GASM Compiler

A custom C compiler targeting GASM.

Repository:
https://github.com/Pnatehc13/C-GASM-Compiler


---

License

See the repository for the current license information.

**One correction from my previous version:** I wouldn't put the README's license section as if the project definitely has a particular license unless we verify the repo's actual `LICENSE` file. Same with the exact build command—we should test/verify it before calling it the official build command.

Once you paste this into `README.md`, **GitHub will render the tables, diagrams, headings, and code blocks properly**.