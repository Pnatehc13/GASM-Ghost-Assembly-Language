#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gasm.h"
#define MAX 4096
#define STACK 256
#define MAXLOOP 32

typedef struct
{
	char op[10];
	int arg;
} Ins;//Instruction FOrmat

unsigned char * prog;// Loading program into memory
int ps = 0;//program size

int stack[STACK];//Stack 
int sp = -1;//
int ip = 0;//Instruction pointer


int retstack[STACK];
int rsp = -1;
void pushrsp()
{
	rsp++;
	retstack[rsp] = ip+4;
}
int poprsp()
{
	return retstack[rsp--];
}


int loopstk[MAXLOOP];
int lsp = -1;


int reg[4];

void push(int v)
{
	if(sp >= STACK-1)
	{
		printf("ERROR: Stack Overflow!!");
		exit(1);
	}
	stack[++sp] = v;
}

int pop()
{
	if(sp>=0)return stack[sp--];
	printf("Stack Underflow");
	exit(1);
}

void pushloop(int v)
{
	if(lsp<MAXLOOP)loopstk[++lsp] = v;
}
void poploop()
{
	lsp--;
}

void loader(char* path)
{
	FILE * f = fopen(path,"rb");
	if(!f)
	{
		perror("File not Found!!");
		exit(0);
	}
	fseek(f,0,SEEK_END);
	ps = ftell(f);
	rewind(f);
	prog = (unsigned char*)malloc(ps*sizeof(unsigned char));
	if(prog == NULL)
	{
		printf("CRITICAL ERROR: Failed to allocate memory for program.\n");
		exit(1);
	}
	fread(prog,1,ps,f);
	fclose(f);
}


int main(int argc,char** argv)
{
	if(argc < 2)
	{
		printf("Usage: ./");
		return 1;
	}
			
	loader(argv[1]);
	
	while(ip<ps)
	{
		unsigned char opcode = prog[ip];
		
		switch(opcode)	
		{
			case HALT:
			{
				return 0;
			}
			case PUSH:
			{
				int arg = *(int*)&prog[ip+1];
				push(arg);
				ip+=5;
				break;
			}
			case POP:
			{
				pop();
				ip+=1;
				break;
			}
			case ADD:
			{
				int a =pop();
				int b = pop();
				push(a+b);
				ip+=1;
				break;
			}
			case SUB:
			{
				int a = pop();
				int b = pop();
				push(b - a);
				ip+=1;
				break;
			}
			case MUL:
			{
				int a = pop();
				int b = pop();
				push(b*a);
				ip+=1;
				break;
			}
			case DIV:
			{
				int a = pop();
				int b = pop();
				if(a==0)
				{
					printf("ERROR: Division by 0");
					exit(1);
				}
				push(b/a);
				ip+=1;
				break;
			}
			case STORE:
			{
				int ri = *(int*)&prog[ip+1];
				reg[ri] = pop();
				ip+=5;
				break;
			}
			case LOAD:
			{
				int ri = *(int*)&prog[ip+1];
				push(reg[ri]);
				ip+=5;
				break;
			}
			case START_LOOP:
			{
				pushloop(ip+1);
				ip+=1;
				break;
			}
			case END_LOOP:
			{
				int t = pop();
				if(t != 0) ip = loopstk[lsp];
				else{poploop();ip+=1;}  
				break;
			}
			case PRINT:
			{
				printf(">> %d\n",stack[sp]);
				ip+=1;
				break;
			}
			case PRINT_C:
			{
				int val = pop();
				printf("%c",val);
				ip+=1;
				break;
			}
			case PRINT_STR:
			{
				int addr = pop();
				char* s = (char*)&prog[addr];
				printf("%s",s);
				ip+=1;
				break;
			}
			case JMP:
			{
				ip = *(int*)&prog[ip+1];
				break;
			}
			case CMP:
			{
				int a = pop();
				int b = pop();
				if(a==b)push(1);
				else push(0);
				ip+=1;
				break;
			}
			case JE:
			{
				int target = *(int*)&prog[ip+1];
				int a = pop();
				if(a == 1)ip = target;
				else ip+=5;
				break;
			}
			case JNE:
			{
				int target = *(int *)&prog[ip+1];
				int a = pop();
				if(a!=1)ip = target;
				else ip+=5;
				break;
			}
			case DUP:
			{
				push(stack[sp]);
				ip+=1;
				break;
			}
			case DUP2:
			{
				int a = pop();
				int b = pop();
				push(b);
				push(a);
				push(b);
				push(a);
				ip+=1;
				break;
			}
			case SWAP:
			{
				int a = pop();
				int b = pop();
				push(a);
				push(b);
				ip+=1;
				break;
			}
			case READ_INT:
			{
				int val;
				scanf("%d",&val);
				push(val);
				ip+=1;
				break;
			}
			case CALL:
			{
				pushrsp();
				ip = *(int*)&prog[ip+1];
				break;
			}
			case RET:
			{
				ip = poprsp()+1;
				break;
			}
			default: break;
		}
	}

	free(prog);
	return 0;
}



