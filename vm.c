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
#define BACK_BUFFER 0x1D800
#define RESERVED    0x1E800
#define SCREEN_W 64
#define SCREEN_H 64



typedef struct
{
	unsigned char* mem;
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
	vm->mem = (unsigned char*)malloc(MEM_SIZE*sizeof(unsigned char));
	vm->sp = STACK_START;
	vm->rsp = RS_START;
	vm->lsp = LS_START;
	vm->ip = CODE_START;
	vm->running =1;
	return vm;	
}

void vm_panic(char * msg)
{
	printf("Panic:%s",msg);
	exit(1);
}

int vmread(VM* vm,int addr)
{
	if(addr<0||addr+4 > MEM_SIZE)
	{
		vm_panic("Access Out of Bounds!!");
	}
	return (int)(
		(vm->mem[addr])|(vm->mem[addr+1]<<8)|(vm->mem[addr+2]<<16)|(vm->mem[addr+3]<<24)
	);
}

void vmwrite(VM* vm,int addr,int val)
{
	if(addr<0||addr+4>MEM_SIZE)
	{
		vm_panic("Access Out of Bounds!");
	}
	vm->mem[addr] = (unsigned char)(val & 0xFF);
	vm->mem[addr+1] = (unsigned char)((val >> 8) & 0xFF);
	vm->mem[addr+2] = (unsigned char)((val >> 16) & 0xFF);
	vm->mem[addr+3] = (unsigned char)((val >> 24) & 0xFF);
}


unsigned int palette[] = { 0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF };
unsigned int pixel[SCREEN_H*SCREEN_W];



void push(VM* vm,int v)
{
	if(vm->sp + 4 >= RS_START)
	{
		vm_panic("Stack Overflow!!");
	}
	vmwrite(vm, vm->sp, v);
	vm->sp += 4;
}

int pop(VM* vm)
{
	vm->sp -= 4;
	if(vm->sp < STACK_START)
	{
		vm_panic("Stack Underflow!!");
	}
	return vmread(vm, vm->sp);
}

void pushrsp(VM*vm,int val)
{
	vm->rsp -= 4;
	if(vm->rsp <= vm->sp)
	{
		vm_panic("Return Stack Overflow!!");
	}
	vmwrite(vm, vm->rsp, val);
}

int poprsp(VM*vm)
{
	if(vm->rsp >= VRAM_START)
	{
		vm_panic("Return Stack Underflow!!");
	}
	int val = vmread(vm, vm->rsp);
	vm->rsp += 4;
	return val;
}

void vmwrite8(VM* vm, int addr, unsigned char val) {
    if (addr < 0 || addr >= MEM_SIZE) {
        vm_panic("Access Out of Bounds (8-bit write)!");
    }
    vm->mem[addr] = val;
}

unsigned char vmread8(VM* vm, int addr) {
    if (addr < 0 || addr >= MEM_SIZE) {
        vm_panic("Access Out of Bounds (8-bit read)!");
    }
    return vm->mem[addr];
}

void pushloop(VM*vm,int v)
{
	if(vm->lsp>=STACK_START)
	{
		vm_panic("Loop Stack Overflow!!");
	}
	vmwrite(vm, vm->lsp, v);
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
	int i = 0;
	for(int y = 0; y < SCREEN_H; y++)
	{
		for(int x = 0; x < SCREEN_W; x++)
		{
			int index = VRAM_START + (y * SCREEN_W) + x;
			unsigned char val = vmread8(vm, index);
			val = val % 5;
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

	StretchDIBits(vm->hdc, 0, 0, 640, 640, 0, 0, SCREEN_W, SCREEN_H, pixel, &bmi, DIB_RGB_COLORS, SRCCOPY);
}


void handle_halt(VM* vm)
{
	vm->running = 0;
}
void handle_draw(VM* vm)
{
	int c = pop(vm);
	int y = pop(vm);
	int x = pop(vm);
	if(x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H)
	{
		int index = BACK_BUFFER + (y * SCREEN_W) + x;
		vmwrite8(vm, index, (unsigned char)c);
	}
}

void handle_push(VM* vm)
{
	int arg = vmread(vm, vm->ip + 1);
	push(vm, arg);
}

void handle_pop(VM* vm)
{
	pop(vm);
}

void handle_key(VM* vm)
{
	int k = pop(vm);
	if(GetAsyncKeyState(k) & 0x8000) push(vm, 1);
	else push(vm, 0);
}

void handle_cls(VM* vm)
{
	for(int i = 0; i < 4096; i++) {
		vmwrite8(vm, VRAM_START + i, 0);
		vmwrite8(vm, BACK_BUFFER + i, 0);
	}
}

void handle_sleep(VM* vm)
{
	int m = pop(vm);
	#ifdef _WIN32
		Sleep(m);
	#else 
		usleep(m * 1000);
	#endif
}

void handle_show(VM* vm)
{
	for(int i = 0; i < 4096; i++) {
		vmwrite8(vm, VRAM_START + i, vmread8(vm, BACK_BUFFER + i));
	}
	render(vm);
}

void handle_add(VM* vm)
{
	int a = pop(vm);
	int b = pop(vm);
	push(vm, a + b);
}


void handle_sub(VM* vm)
{
	int a = pop(vm);
	int b = pop(vm);
	push(vm, b - a);
}

void handle_mul(VM* vm)
{
	int a = pop(vm);
	int b = pop(vm);
	push(vm, b * a);
}
void handle_div(VM* vm)
{
	int a = pop(vm);
	int b = pop(vm);
	if(a==0)
	{
		printf("ERROR: Division by 0");
		exit(1);
	}
	push(vm,b/a);
}


void handle_store(VM* vm)
{
	int ri = vmread(vm,vm->ip + 1);
	int val = pop(vm);
	int reg_addr = REG_START + (ri * 4);
	if(reg_addr < REG_START || reg_addr + 4 > LS_START) {
		vm_panic("Register access out of bounds!");
	}
	vmwrite(vm, reg_addr, val);
}
void handle_load(VM* vm)
{
	int ri = vmread(vm, vm->ip + 1);
	int reg_addr = REG_START + (ri * 4);
	if(reg_addr < REG_START || reg_addr + 4 > LS_START) {
		vm_panic("Register access out of bounds!");
	}
	int val = vmread(vm, reg_addr);
	push(vm, val);
}


void handle_startloop(VM* vm)
{
	pushloop(vm, vm->ip + 1);
}
void handle_endloop(VM* vm)
{
	int t = pop(vm);
	if(t != 0) vm->ip = vmread(vm, vm->lsp - 4);
	else { 
		poploop(vm); 
		vm->ip += 1;
	}  
}
void handle_print(VM* vm)
{
	printf(">> %d\n", vmread(vm, vm->sp - 4));
}
void handle_printc(VM* vm)
{
	int val = pop(vm);
	printf("%c", val);
}
void handle_printstr(VM* vm)
{
	int addr = pop(vm);
	// Safe string printing: stop at null or end of memory
	for(int i = addr; i < MEM_SIZE; i++) {
		char c = (char)vmread8(vm, i);
		if(c == '\0') break;
		printf("%c", c);
	}
}



void handle_jmp(VM* vm)
{
	vm->ip = vmread(vm, vm->ip + 1);
}
void handle_cmp(VM* vm)
{
	int a = pop(vm);
	int b = pop(vm);
	if(a == b) push(vm, 1);
	else push(vm, 0);
}
void handle_je(VM* vm)
{
	int target = vmread(vm, vm->ip + 1);
	int a = pop(vm);
	if(a == 1) vm->ip = target;
	else vm->ip += 5;
}
void handle_jne(VM* vm)
{
	int target = vmread(vm, vm->ip + 1);
	int a = pop(vm);
	if(a != 1) vm->ip = target;
	else vm->ip += 5;
}

void handle_dup(VM* vm)
{
	int val = vmread(vm, vm->sp - 4);
	push(vm, val);
}
void handle_dup2(VM* vm)
{
	int a = pop(vm);
	int b = pop(vm);
	push(vm, b);
	push(vm, a);
	push(vm, b);
	push(vm, a);
}
void handle_swap(VM* vm)
{
	int a = pop(vm);
	int b = pop(vm);
	push(vm, a);
	push(vm, b);
}
void handle_readint(VM* vm)
{
	int val;
	scanf("%d", &val);
	push(vm, val);
}

void handle_call(VM* vm)
{
	pushrsp(vm, vm->ip + 5);
	vm->ip = vmread(vm, vm->ip + 1);
}
void handle_ret(VM* vm)
{
	vm->ip = poprsp(vm);
}
void handle_rand(VM* vm)
{
	int u = pop(vm);
	int l = pop(vm);
	if(u <= l) { vm_panic("RAND: Upper bound must be greater than lower bound!"); }
	int num = rand() % (u - l + 1) + l;
	push(vm, num);
}

typedef void (*Handler)(VM* vm);
Handler dispatch_table[] = {
	[HALT] = handle_halt,
	[PUSH] = handle_push,
	[POP] = handle_pop,
	[ADD] = handle_add,
    [SUB] = handle_sub,
    [MUL] = handle_mul,
    [DIV] = handle_div,
    [STORE] = handle_store,      
    [LOAD] = handle_load,       
    [START_LOOP] = handle_startloop, 
    [END_LOOP] = handle_endloop,  
    [JMP] = handle_jmp,       
    [CMP] = handle_cmp,       
    [PRINT] = handle_print,     
    [JE] = handle_je,
    [DUP] = handle_dup,
    [DUP2] = handle_dup2,
    [SWAP] = handle_swap,
    [PRINT_C] = handle_printc,
    [PRINT_STR] = handle_printstr,
    [READ_INT] = handle_readint,
    [CALL] = handle_call,
    [RET] = handle_ret,
    [JNE]= handle_jne,
    [DRAW]= handle_draw,
	[SLEEP] = handle_sleep,
	[CLS]= handle_cls,
	[SHOW]= handle_show,
	[RAND] = handle_rand,
	[KEY] = handle_key
};

void cpu(VM* vm)
{
	unsigned char opcode = vmread8(vm, vm->ip);
	dispatch_table[opcode](vm);
	if(!OCT[opcode].isflow)
	{
		vm->ip+=OCT[opcode].size;
	}
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
	ReleaseDC(machine->hwnd, machine->hdc);
	if (machine->mem) free(machine->mem);
	free(machine);
	return 0;
}






