#ifndef GASM_H
#define GASM_H
#define CHARSET " .:#@"
#include <stdint.h>
// Memory layout (16MB)
#define MEM_SIZE        (16 * 1024 * 1024)

#define CODE_START      0x000000   // Instructions
#define CODE_SIZE       (1 * 1024 * 1024)

#define DATA_START      0x100000   // Global variables
#define DATA_SIZE       (1 * 1024 * 1024)

#define HEAP_START      0x200000   // malloc'd variables and WAD data
#define HEAP_SIZE       (10 * 1024 * 1024)

#define STACK_START     0xC00000   // Local variables, return addresses
#define STACK_SIZE      (2 * 1024 * 1024)
#define STACK_TOP       (STACK_START + STACK_SIZE)  // Stack grows DOWN from here

#define SCREEN_W        320
#define SCREEN_H        240
#define VRAM_START      0xE00000   // Screen pixels
#define VRAM_SIZE       (SCREEN_W * SCREEN_H * sizeof(uint32_t))
#define BACK_BUFFER     (VRAM_START + VRAM_SIZE)
#define INPUT_START     0xF00000   // 256 bytes for key states


typedef struct 
{
	int op;
	char* name;
	int hasarg;
	int size;
	int isflow;
} opcode;



typedef enum {
    HALT = 0,       
    PUSH = 1,       
    POP = 2,        
    ADD = 3,
    SUB = 4,
    MUL = 5,
    DIV = 6,
    STORE = 7,      
    LOAD = 8,       
    JMP = 9,       
    CMP = 10,       
    PRINT = 11,     
    JE = 12,
    DUP = 13,
    DUP2 = 14,
    SWAP = 15,
    PRINT_C = 16,
    PRINT_STR = 17,
    READ_INT = 18,
    FUNC = 19,
    CALL = 20,
    RET = 21,
    JNE = 22,
    LABEL = 23,
    DRAW = 24,
	SLEEP = 25,
	CLS = 26,
	SHOW = 27,
	RAND = 28,
	KEY = 29,
	INCLUDE = 30,
	POKE = 31,
	PEEK = 32,
	GETSP = 33,
	GETBP = 34,
	AND = 35,
	OR = 36,
	NOT = 37,
	XOR = 38,
	SHL = 39,
	SHR = 40,
	MOD = 41,
	PEEKL = 42,
	POKEL = 43,
	PEEK8 = 44,
	POKE8 = 45,
	JL = 46,
	JG = 47,
	JLE = 48,
	JGE = 49,
	FOPEN = 50,
	FCLOSE = 51,
	FREAD = 52,
	FWRITE = 53,
	FSEEK = 54,
	FTELL = 55,
	FSIZE = 56,
	FEOF = 57
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
    {FUNC, "FUNC", 0, 0,0},  
    {CALL, "CALL", 1, 5,1},
    {RET, "RET", 0, 1,1},
    {JNE, "JNE", 1, 5,1},//
    {LABEL, "LABEL", 0, 0,0}, 
    {DRAW,"DRAW",0,1,0},
    {SLEEP,"SLEEP",0,1,0},
    {CLS,"CLS",0,1,0},
    {SHOW,"SHOW",0,1,0},
    {RAND,"RAND",0,1,0},
    {KEY,"KEY",0,1,0},
    {INCLUDE,"INCLUDE",1,0,0},
    {POKE,"POKE",0,1,0},
    {PEEK,"PEEK",0,1,0},
    {GETSP,"GETSP",0,1,0},
    {GETBP,"GETBP",0,1,0},
    {AND, "AND", 0, 1, 0},
    {OR, "OR", 0, 1, 0},
    {NOT, "NOT", 0, 1, 0},
    {XOR, "XOR", 0, 1, 0},
    {SHL, "SHL", 0, 1, 0},
    {SHR, "SHR", 0, 1, 0},
    {MOD, "MOD", 0, 1, 0},
    {PEEKL,"PEEKL",1,5,0},
    {POKEL,"POKEL",1,5,0},
    {PEEK8,"PEEK8",0,1,0},
    {POKE8,"POKE8",0,1,0},
    {JL, "JL", 1, 5, 1},
    {JG, "JG", 1, 5, 1},
    {JLE, "JLE", 1, 5, 1},
    {JGE, "JGE", 1, 5, 1},
    {FOPEN , "FOPEN", 0,1,0},
    {FCLOSE , "FCLOSE", 0,1,0},
    {FREAD , "FREAD", 0,1,0},
    {FWRITE , "FWRITE", 0,1,0},
    {FSEEK , "FSEEK", 0,1,0},
    {FTELL , "FTELL", 0,1,0},
    {FSIZE , "FSIZE", 0,1,0},
    {FEOF , "FEOF", 0,1,0},
    {-1, "", 0, 0,0}       
};


#endif
