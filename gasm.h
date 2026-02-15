#ifndef GASM_H
#define GASM_H
#define CHARSET " .:#@"


typedef struct 
{
	int op;
	char* name;
	int hasarg;
	int size;
	int isflow;
} opcode;


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
    STORE = 7,      
    LOAD = 8,       
    START_LOOP = 9, 
    END_LOOP = 10,  
    JMP = 11,       
    CMP = 12,       
    PRINT = 13,     
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
    LABEL = 25,
    DRAW = 26,
	SLEEP = 27,
	CLS = 28,
	SHOW = 29,
	RAND = 30,
	KEY = 31,
	INCLUDE = 32
} Opcode;


static const opcode OCT[] = {
    {HALT, "HALT", 0, 1,1},
    {PUSH, "PUSH", 1, 5,0},
    {POP, "POP", 0, 1,0},
    {ADD, "ADD", 0, 1,0},
    {SUB, "SUB", 0, 1,0},
    {MUL, "MUL", 0, 1,0},
    {DIV, "DIV", 0, 1,0},
    {STORE, "STORE", 1, 5,0},
    {LOAD, "LOAD", 1, 5,0},
    {START_LOOP, "START_LOOP", 0, 1,0},
    {END_LOOP, "END_LOOP", 0, 1,1},
    {JMP, "JMP", 1, 5,1},//
    {CMP, "CMP", 0, 1,0},
    {PRINT, "PRINT", 0, 1,0},
    {JE, "JE", 1, 5,1},//
    {DUP, "DUP", 0, 1,0},
    {DUP2, "DUP2", 0, 1,0},
    {SWAP, "SWAP", 0, 1,0},
    {PRINT_C, "PRINT_C", 0, 1,0},
    {PRINT_STR, "PRINT_STR", 0, 1,0},
    {READ_INT, "READ_INT", 0, 1,0},
    {FUNC, "FUNC", 0, 0,0},   // Size 0 because these are metadata/labels
    {CALL, "CALL", 1, 5,1},//
    {RET, "RET", 0, 1,1},
    {JNE, "JNE", 1, 5,1},//
    {LABEL, "LABEL", 0, 0,0}, // Size 0 because these are metadata/labels
    {DRAW,"DRAW",0,1,0},
    {SLEEP,"SLEEP",0,1,0},
    {CLS,"CLS",0,1,0},
    {SHOW,"SHOW",0,1,0},
    {RAND,"RAND",1,5,0},
	{KEY,"KEY",0,1,0},
	{INCLUDE,"INCLUDE",1,0,0},
    {-1, "", 0, 0,0}        // Sentinel value to mark the end of the array
};


#endif
