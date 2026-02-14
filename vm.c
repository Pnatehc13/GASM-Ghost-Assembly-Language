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

#define MEM_SIZE    131072
#define CODE_START  0x00000
#define DATA_START  0x08000
#define HEAP_START  0x10000
#define REG_START   0x14000  // Registers start here
#define LS_START    0x14400  // Loop Stack starts here
#define STACK_START 0x14800  // Main Stack starts here
#define RS_START    0x18800  // Return Stack (grows down)
#define VRAM_START  0x1C800
#define VRAM_END    0x1D800
#define BACK_BUFFER 0x1D800
#define SCREEN_W 64
#define SCREEN_H 64



typedef struct
{
	unsigned char mem[MEM_SIZE];
	int ip;//Instruction pointer
	int sp;//Stack pointer
	int lsp;//Loop Stack Pointer
	int rsp;//Return Stack pointer
	int ps;//program size
	int running;
	HWND hwnd;
	HDC hdc;
}VM;

VM* machine;


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


unsigned int palette[] = { 0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF };
unsigned int pixel[SCREEN_H*SCREEN_W];
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
	
	int i =0;
	for(int y =0;y<SCREEN_H;y++)
	{
		for(int x =0;x<SCREEN_W;x++)
		{
			int index = VRAM_START + (y*64)+x;
			unsigned char val = vm->mem[index];
			val = val%5;
			pixel[i++] = palette[val]; 
		}
	}
	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = SCREEN_W;
	bmi.bmiHeader.biHeight = -SCREEN_H;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount= 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	StretchDIBits(vm->hdc,0,0,640,640,0,0,SCREEN_W,SCREEN_H,pixel,&bmi,DIB_RGB_COLORS,SRCCOPY);
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
					int index = BACK_BUFFER + (y*64)+x;
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
			case KEY:
			{
				int k = pop(vm);
				if(GetAsyncKeyState(k)&0x8000)push(vm,1);
				else push(vm,0);
				break;
			}
			case CLS:
			{
				memset(&vm->mem[VRAM_START], 0, 4096);
				memset(&vm->mem[BACK_BUFFER],0, 4096);
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
				memcpy(&vm->mem[VRAM_START],&vm->mem[BACK_BUFFER],4096);
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


const char g_szClassName[] = "Window";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
        	machine->running = 0;
            PostQuitMessage(0);
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Draw a red rectangle to prove we have graphics
            HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
            FillRect(hdc, &ps.rcPaint, brush);
            DeleteObject(brush);
            
            EndPaint(hwnd, &ps);
        }
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}



int main(int argc,char** argv)
{

	WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
	
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        printf("Window Registration Failed!");
        return 0;
    }
	hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "GASM - Native Window Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 640,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if(hwnd == NULL)
    {
        printf("Window Creation Failed!");
        return 0;
    }
	
	
	ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
	
	if(argc < 2)
	{
		printf("Usage: ./");
		return 1;
	}
	machine = new_Vm();
	machine->hwnd = hwnd;
	machine->hdc = GetDC(hwnd);	
	loader(machine,argv[1]);

	while(machine->running && machine->ip < machine->ps) 
	{	
		if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			if(Msg.message == WM_QUIT)machine->running = 0;
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		cpu(machine);
	}
	ReleaseDC(machine->hwnd,machine->hdc);
	free(machine);
	return 0;
}






