#ifndef GASM_H
#define GASM_H

// The "Menu" of commands
// Both the Assembler and VM must see this exact list.
typedef enum {
    HALT = 0,       // Stop the machine
    PUSH = 1,       // Push number (followed by arg)
    POP = 2,        // Pop stack
    ADD = 3,
    SUB = 4,
    MUL = 5,
    DIV = 6,
    STORE = 7,      // Store to Reg (followed by Reg ID)
    LOAD = 8,       // Load from Reg (followed by Reg ID)
    START_LOOP = 9, // Loop Start
    END_LOOP = 10,  // Loop End
    JMP = 11,       // Jump (followed by line number)
    CMP = 12,       // Compare top 2 items
    PRINT = 13,      // Print top of stack
    JE = 14,
    DUP = 15,
    DUP2 = 16,
    SWAP = 17,
    PRINT_C = 18,
    PRINT_STR = 19,
    READ_INT = 20,
    FUNC = 21,
    CALL = 22,
    RET = 23,
    JNE = 24,
    LABEL = 25
} Opcode;

#endif
