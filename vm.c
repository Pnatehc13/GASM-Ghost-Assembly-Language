#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "gasm.h"
#define MAX 4096
#define STACK 256
#define MAXLOOP 32

typedef struct
{
	char op[10];
	int arg;
} Ins;//Instruction FOrmat

#define MEM_SIZE    65536
#define CODE_START  0x0000
#define DATA_START  0x4000
#define HEAP_START  0x6000
#define REG_START   0x8000  // Registers start here
#define LS_START    0x8100  // Loop Stack starts here
#define STACK_START 0x9000  // Main Stack starts here
#define RS_START    0xFFFF  // Return Stack (grows down)
#define VRAM_START  0xC000
#define VRAM_END    0xC400
#define BACK_BUFFER 0xD000
#define SCREEN_W 32
#define SCREEN_H 32



typedef struct
{
	unsigned char mem[MEM_SIZE];
	int ip;//Instruction pointer
	int sp;//Stack pointer
	int lsp;//Loop Stack Pointer
	int rsp;//Return Stack pointer
	int ps;//program size
	int running;
}VM;




VM* new_Vm()
{
	srand((unsigned int)time(NULL));	
	VM* vm = (VM *)malloc(sizeof(VM));
	vm->sp = STACK_START;
	vm->rsp = RS_START;
	vm->lsp = LS_START;
	vm->ip = CODE_START;
	vm->running =1;
	return vm;	
}

void push(VM* vm,int v)
{
	if(vm->sp +4 >= RS_START)
	{
		printf("ERROR: Stack Overflow!!");
		exit(1);
	}
	*(int*)&vm->mem[vm->sp] = v;
	vm->sp+=4;
}

int pop(VM* vm)
{
	vm->sp-=4;
	
	if(vm->sp<STACK_START)
	{
		printf("Stack Underflow");
		exit(1);
	}
	return *(int*)&vm->mem[vm->sp];
}

void pushrsp(VM*vm,int val)
{
	vm->rsp-=4;
	if(vm->rsp<=vm->sp)
	{
		printf("Error:!!");
		exit(1);
	}
	*(int*)&vm->mem[vm->rsp] = val;
}
int poprsp(VM*vm)
{
	int val = *(int*)&vm->mem[vm->rsp];
	vm->rsp+=4;
	return val;
}

void pushloop(VM*vm,int v)
{
	if(vm->lsp>=STACK_START)
	{
		printf("Error :Insuffient Space!! ");
		exit(1);
	}
	*(int*)&vm->mem[vm->lsp] = v;
	vm->lsp+=4;
}
void poploop(VM*vm)
{
	vm->lsp-=4;
}

void loader(VM* vm,char* path)
{
	FILE * f = fopen(path,"rb");
	if(!f)
	{
		perror("File not Found!!");
		exit(0);
	}
	fseek(f,0,SEEK_END);
	vm->ps = ftell(f);
	rewind(f);

	if(vm->ps >DATA_START)
	{
		printf("CRITICAL ERROR: Failed to allocate memory for program.\n");
		exit(1);
	}
	fread(&vm->mem[CODE_START],1,vm->ps,f);
	fclose(f);
}

int getincr(unsigned char op) {
    for (int i = 0; OCT[i].op != -1; i++) {
        if (OCT[i].op == op) return OCT[i].size;
    }
    return 1;
}



void render(VM* vm)
{
	printf("\033[H");
	char buffer[3000];
	int i =0;
	for(int y =0;y<SCREEN_H;y++)
	{
		for(int x =0;x<SCREEN_W;x++)
		{
			int index = VRAM_START + (y*32)+x;
			unsigned char val = vm->mem[index];
			if(val > 4)val = 4;
			buffer[i++] = CHARSET[val];
			buffer[i++] = CHARSET[val];
		}
		buffer[i++] = '\n';
	}
	buffer[i]='\0';
	printf("%s",buffer);
}



void cpu(VM* vm)
{
	int ip = vm->ip;
	int currip = ip;
	unsigned char opcode = vm->mem[ip];
	int incr = getincr(opcode);
		
	switch(opcode)	
		{
			case HALT:
			{
				vm->running = 0;
				return;
			}
			case DRAW:
			{
				int c = pop(vm);
				int y = pop(vm);
				int x = pop(vm);
				if(x>=0 && x<SCREEN_W && y>=0 && y< SCREEN_H)
				{
					int index = BACK_BUFFER + (y*32)+x;
					vm->mem[index] = (unsigned char) c;
				}
				break;
			}
			case PUSH:
			{
				int arg = *(int*)&vm->mem[ip+1];
				push(vm,arg);
				break;
			}
			case POP:
			{
				pop(vm);
				break;
			}
			case CLS:
			{
				memset(&vm->mem[VRAM_START], 0, 1024);
				memset(&vm->mem[BACK_BUFFER],0,1024);
				break;
			}
			case SLEEP:
			{
				int m = pop(vm);
				#ifdef _WIN32
					Sleep(m);
				#else 
					usleep(m*1000);
				#endif
				break;
			}
			case SHOW:
			{
				memcpy(&vm->mem[VRAM_START],&vm->mem[BACK_BUFFER],1024);
				render(vm);
				break;
			}
			case ADD:
			{
				int a =pop(vm);
				int b = pop(vm);
				push(vm,a+b);
				break;
			}
			case SUB:
			{
				int a = pop(vm);
				int b = pop(vm);
				push(vm,b - a);
				break;
			}
			case MUL:
			{
				int a = pop(vm);
				int b = pop(vm);
				push(vm,b*a);
				break;
			}
			case DIV:
			{
				int a = pop(vm);
				int b = pop(vm);
				if(a==0)
				{
					printf("ERROR: Division by 0");
					exit(1);
				}
				push(vm,b/a);
				break;
			}
			case STORE:
			{
				int ri = *(int*)&vm->mem[ip+1];
				int val = pop(vm);
				*(int*)&vm->mem[REG_START+(ri*4)]= val;
				break;
			}
			case LOAD:
			{
				int ri = *(int*)&vm->mem[ip+1];
				int val = *(int*)&vm->mem[REG_START+(ri*4)];
				push(vm,val);
				break;
			}
			case START_LOOP:
			{
				pushloop(vm,ip+1);
				break;
			}
			case END_LOOP:
			{
				int t = pop(vm);
				if(t != 0) ip = *(int*)&vm->mem[vm->lsp-4];
				else{poploop(vm);}  
				break;
			}
			case PRINT:
			{
				printf(">> %d\n",*(int*)&vm->mem[vm->sp-4]);
				break;
			}
			case PRINT_C:
			{
				int val = pop(vm);
				printf("%c",val);
				break;
			}
			case PRINT_STR:
			{
				int addr = pop(vm);
				char* s = (char*)&vm->mem[addr];
				printf("%s",s);
				break;
			}
			case JMP:
			{
				ip = *(int*)&vm->mem[ip+1];
				break;
			}
			case CMP:
			{
				int a = pop(vm);
				int b = pop(vm);
				if(a==b)push(vm,1);
				else push(vm,0);
				break;
			}
			case JE:
			{
				int target = *(int*)&vm->mem[ip+1];
				int a = pop(vm);
				if(a == 1)ip = target;
				break;
			}
			case JNE:
			{
				int target = *(int *)&vm->mem[ip+1];
				int a = pop(vm);
				if(a!=1)ip = target;
				break;
			}
			case DUP:
			{
				int val = *(int*)&vm->mem[vm->sp-4];
				push(vm,val);
				break;
			}
			case DUP2:
			{
				int a = pop(vm);
				int b = pop(vm);
				push(vm,b);
				push(vm,a);
				push(vm,b);
				push(vm,a);
				break;
			}
			case SWAP:
			{
				int a = pop(vm);
				int b = pop(vm);
				push(vm,a);
				push(vm,b);
				break;
			}
			case READ_INT:
			{
				int val;
				scanf("%d",&val);
				push(vm,val);
				break;
			}
			case CALL:
			{
				pushrsp(vm,ip+5);
				ip = *(int*)&vm->mem[ip+1];
				break;
			}
			case RET:
			{
				ip = poprsp(vm);
				break;
			}
			case RAND:
			{
				int u = pop(vm);
				int l = pop(vm);
				if(u<=l){printf("Error!!");exit(1);}
				int num = rand()%(u-l+1)+l;
				push(vm,num);
			}
			default: break;
		}
		
	if(ip == currip)vm->ip = currip+incr;
	else vm->ip = ip;
	
}

int main(int argc,char** argv)
{
	printf("\033[?25l");
	if(argc < 2)
	{
		printf("Usage: ./");
		return 1;
	}
	VM* machine = new_Vm();	
	loader(machine,argv[1]);
	while(machine->running && machine->ip < machine->ps) 
	{	
		cpu(machine);
	}
	printf("\033[?25h");
	render(machine);
	free(machine);
	return 0;
}






