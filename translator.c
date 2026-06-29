#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_opcode_value(const char* name) {
    if (strcmp(name, "HALT") == 0) return 0;
    if (strcmp(name, "PUSH") == 0) return 1;
    if (strcmp(name, "POP") == 0) return 2;
    if (strcmp(name, "ADD") == 0) return 3;
    if (strcmp(name, "SUB") == 0) return 4;
    if (strcmp(name, "MUL") == 0) return 5;
    if (strcmp(name, "DIV") == 0) return 6;
    if (strcmp(name, "STORE") == 0) return 7;
    if (strcmp(name, "LOAD") == 0) return 8;
    if (strcmp(name, "JMP") == 0) return 9;
    if (strcmp(name, "CMP") == 0) return 10;
    if (strcmp(name, "PRINT") == 0) return 11;
    if (strcmp(name, "JE") == 0) return 12; // Opcode 12
    if (strcmp(name, "DUP") == 0) return 13;
    if (strcmp(name, "DUP2") == 0) return 14;
    if (strcmp(name, "SWAP") == 0) return 15;
    if (strcmp(name, "PRINT_C") == 0) return 16;
    if (strcmp(name, "PRINT_STR") == 0) return 17;
    if (strcmp(name, "READ_INT") == 0) return 18;
    if (strcmp(name, "FUNC") == 0) return 19;
    if (strcmp(name, "CALL") == 0) return 20;
    if (strcmp(name, "RET") == 0) return 21;
    if (strcmp(name, "JNE") == 0) return 22;
    if (strcmp(name, "LABEL") == 0) return 23;
    if (strcmp(name, "DRAW") == 0) return 24;
    if (strcmp(name, "SLEEP") == 0) return 25;
    if (strcmp(name, "CLS") == 0) return 26;
    if (strcmp(name, "SHOW") == 0) return 27;
    if (strcmp(name, "RAND") == 0) return 28;
    if (strcmp(name, "KEY") == 0) return 29;
    if (strcmp(name, "INCLUDE") == 0) return 30;
    if (strcmp(name, "POKE") == 0) return 31;
    if (strcmp(name, "PEEK") == 0) return 32;
    if (strcmp(name, "GETSP") == 0)return 33;
    if (strcmp(name, "GETBP") == 0) return 34;
    if (strcmp(name, "AND") == 0) return 35;
    if (strcmp(name, "OR") == 0) return 36;
    if (strcmp(name, "NOT") == 0) return 37;
    if (strcmp(name, "XOR") == 0) return 38;
    if (strcmp(name, "SHL") == 0) return 39;
    if (strcmp(name, "SHR") == 0) return 40;
    if (strcmp(name, "MOD") == 0) return 41;
    if (strcmp(name, "PEEKL") == 0) return 42;
    if (strcmp(name, "POKEL") == 0) return 43;
    if (strcmp(name, "PEEK8") == 0) return 44;
    if (strcmp(name, "POKE8") == 0) return 45;
    if (strcmp(name, "JL") == 0) return 46;
    if (strcmp(name, "JG") == 0) return 47;
    if (strcmp(name, "JLE") == 0) return 48;
    if (strcmp(name, "JGE") == 0) return 49;
    return -1;
}

int has_argument(int op) {
    // Returns 1 if the opcode takes a payload word
    if (op == 1 || op == 7 || op == 8 || op == 9 || (op >= 11 && op <= 12) || 
        op == 19 || op == 20 || op == 22 || op == 23 || op == 25 || 
        op == 28 || op == 30 || (op >= 42 && op <= 43) || (op >= 46 && op <= 49)) {
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <output.gasm>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error opening text file.\n");
        return 1;
    }

    char line[256];
    char op_name[64];
    int arg_val;
    int ip = 0;

    printf("=== TEXT GASM MEMORY MAPPER ===\n\n");
    printf("IP     | Opcode  | Instruction Line\n");
    printf("-----------------------------------\n");

    while (fgets(line, sizeof(line), file)) {
        // Clear whitespaces and get token
        if (sscanf(line, "%s %d", op_name, &arg_val) < 1) continue;

        // Skip metadata tags that don't take up an instruction slot in memory
        if (strcmp(op_name, "LABEL") == 0 || strcmp(op_name, "FUNC") == 0) {
            printf("[STRIP] | ------- | %s", line);
            continue;
        }

        int op_val = get_opcode_value(op_name);
        if (op_val == -1) {
            printf("[%05d] | ?????   | UNKNOWN INSTR: %s", ip, line);
            ip += 1;
            continue;
        }

        printf("[%05d] | %-7d | %s", ip, op_val, line);
        
        if (has_argument(op_val)) {
            ip += 2; // Opcode + Argument payload
        } else {
            ip += 1; // Opcode only
        }
    }

    fclose(file);
    return 0;
}
