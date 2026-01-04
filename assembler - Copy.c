#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gasm.h"



struct
{
	char name[50];
	int addr;
}symtab[100];
int tsize = 0;
void insert(char* s,int a)
{
	strcpy(symtab[tsize].name,s);
	symtab[tsize].addr = a;
	tsize++;
}
int addr(char*s)
{
	for(int i=0;i<100;i++)
	{
		if(strcmp(symtab[i].name,s)==0)return symtab[i].addr;
	}
	return -1;
}


char strings[100][256];
int sp = -1;
void push(char s[])
{
	sp++;
	strcpy(strings[sp],s);	
}
char* pop()
{
	return strings[sp--];
}
int csize = 0;
int dsize = 0;
int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("Usage: ./");
		return 1;
	}

	FILE *source = fopen(argv[1],"r");
	char bfn[256];
	strcpy(bfn,argv[1]);
	char* dot = strrchr(bfn,'.');
	if(dot)strcpy(dot,".bin");
	else strcat(bfn,".bin");

	FILE *op = fopen(bfn,"wb");

	if(!source || !op)
	{
		perror("File Error!");return 1;
	}

	char line[256];
	printf("Compiling %s to %s...\n",argv[1],bfn);
	while(fgets(line,sizeof(line),source))
	{
		char* c = strchr(line,';');
		if(c)*c='\0';;
		char* token = strtok(line," \n\t");
		if(!token)continue;
		if (strcmp(token, "PUSH") == 0)      { csize+=5; }
        else if (strcmp(token, "POP") == 0)  { csize+=1; }
        else if (strcmp(token, "ADD") == 0)  { csize+=1; }
        else if (strcmp(token, "SUB") == 0)  { csize+=1; }
        else if (strcmp(token, "MUL") == 0)  { csize+=1; }
        else if (strcmp(token, "DIV") == 0)  { csize+=1; }
        else if (strcmp(token, "PRINT") == 0){ csize+=1; }
        else if (strcmp(token, "PRINT_C") == 0){ csize+=1; }
        else if (strcmp(token, "PRINT_STR") == 0){ csize+=1; }
        else if (strcmp(token, "STORE") == 0){ csize+=5; }
        else if (strcmp(token, "LOAD") == 0) { csize+=5; }
        else if (strcmp(token, "START_LOOP") == 0) { csize+=1; }
        else if (strcmp(token, "END_LOOP") == 0)   { csize+=1; }
        else if (strcmp(token, "CMP") == 0)   { csize+=1; }
        else if (strcmp(token, "JMP") == 0)   { csize+=5; }
        else if (strcmp(token, "HALT") == 0)  { csize+=1; }
        else if (strcmp(token, "JE") == 0) {csize+=5; }
        else if (strcmp(token, "DUP") == 0) {csize+=1; }
        else if (strcmp(token, "DUP2") == 0) {csize+=1; }
        else if (strcmp(token, "SWAP") == 0) {csize+=1; }
        else if (strcmp(token, "READ_INT") == 0) {csize+=1; }
        else if (strcmp(token, "FUNC") == 0) {csize+=0; }
        else if (strcmp(token, "CALL") == 0) {csize+=5; }
        else if (strcmp(token, "RET") == 0) {csize+=1; }
        else {
            printf("Error: Unknown Instruction '%s'\n", token);
            continue;
        }
		

		char *str = strtok(NULL,"\n\t");
		if(str && str[0] == '\"')
		{
			push(str);
			dsize+=strlen(str);
		}
		else if(str && strcmp(token,"FUNC")==0)
		{
			insert(str,csize);
		}
				
	}
	rewind(source);
	int curr = csize;
	while(fgets(line,sizeof(line),source))
	{
		char* c = strchr(line,';');
		if(c)*c='\0';

		char* token = strtok(line," \n\t");
		if(!token)continue;

		unsigned char opcode = 0;
		int arg = 0;
		int has_arg = 0;
		
		if (strcmp(token, "PUSH") == 0)      { opcode = PUSH; has_arg = 1; }
        else if (strcmp(token, "POP") == 0)  { opcode = POP; }
        else if (strcmp(token, "ADD") == 0)  { opcode = ADD; }
        else if (strcmp(token, "SUB") == 0)  { opcode = SUB; }
        else if (strcmp(token, "MUL") == 0)  { opcode = MUL; }
        else if (strcmp(token, "DIV") == 0)  { opcode = DIV; }
        else if (strcmp(token, "PRINT") == 0){ opcode = PRINT; }
        else if (strcmp(token, "PRINT_C") == 0){ opcode = PRINT_C; }
        else if (strcmp(token, "PRINT_STR") == 0){ opcode = PRINT_STR; }
        else if (strcmp(token, "STORE") == 0){ opcode = STORE; has_arg = 1; }
        else if (strcmp(token, "LOAD") == 0) { opcode = LOAD; has_arg = 1; }
        else if (strcmp(token, "START_LOOP") == 0) { opcode = START_LOOP; }
        else if (strcmp(token, "END_LOOP") == 0)   { opcode = END_LOOP; }
        else if (strcmp(token, "CMP") == 0)   { opcode = CMP; }
        else if (strcmp(token, "JMP") == 0)   { opcode = JMP; has_arg = 1; }
        else if (strcmp(token, "HALT") == 0)  { opcode = HALT; }
        else if (strcmp(token, "JE") == 0) {opcode = JE; has_arg = 1;}
        else if (strcmp(token, "DUP") == 0) {opcode = DUP; }
        else if (strcmp(token, "DUP2") == 0) {opcode = DUP2; }
        else if (strcmp(token, "SWAP") == 0) {opcode = SWAP; }
        else if (strcmp(token, "READ_INT") == 0) {opcode = READ_INT; }
        else if (strcmp(token, "FUNC") == 0) {continue; }
        else if (strcmp(token, "CALL") == 0) {opcode = CALL;has_arg = 1;}
        else if (strcmp(token, "RET") == 0) {opcode = RET; }
        else {
            printf("Error: Unknown Instruction '%s'\n", token);
            continue;
        }

        fwrite(&opcode,sizeof(unsigned char),1,op);

        if(has_arg)
        {
        	char* arg_str = strtok(NULL,"\n\t");
        	if(arg_str)
        	{
        		if(opcode == STORE || opcode== LOAD)arg = arg_str[0] - 'A';
        		else if(arg_str[0] == '\'')
        		{
        			arg = arg_str[1];
        		} 
        		else if(arg_str[0] == '\"')
        		{
        			arg = curr;
        			curr+=strlen(arg_str)+1;
        		}
        		else if(opcode == CALL)arg = addr(arg_str);
        		else arg = atoi(arg_str);
        		
        	}
        	fwrite(&arg,sizeof(int),1,op);	
   	    }
   	    
	}
	for(int i=0;i<=sp;i++)
	{
		char *s = strings[i];
		s++;
		fwrite(s,strlen(s)-1,1,op);
		char null = '\0';
		fwrite(&null,1,1,op);
	}	
	
	fclose(source);
	fclose(op);
	return 0;
}
