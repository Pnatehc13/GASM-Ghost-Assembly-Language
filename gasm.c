#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gasm.h"

// Define shared variables here
char shared_string_table[MAX_STR_TABLE];
int shared_string_ptr = 0;

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("Usage: gasm <source.gasm>\n");
		return 1;
	}
	char oppath[256];
	strcpy(oppath, argv[1]);
	char* dot = strrchr(oppath, '.');
	if(dot) strcpy(dot, ".bin");
	else strcat(oppath, ".bin");

	assm_start(argv[1]);
	vm_start(oppath);
	return 0;
}

