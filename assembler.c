#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gasm.h"

char* trim(char* s) {
    if(!s) return NULL;
    while(*s == ' ' || *s == '\t') s++;
    if(*s == 0) return s;
    char* back = s + strlen(s) - 1;
    while(back > s && (*back == ' ' || *back == '\t' || *back == '\n' || *back == '\r')) back--;
    *(back+1) = '\0';
    return s;
}

struct
{
	char name[50];
	int addr;
}symtab[10000];
int tsize = 0;
char libs[255][256];
int pl = 0;

void insert(char* s,int a)
{
	strcpy(symtab[tsize].name,s);
	symtab[tsize].addr = a;
	tsize++;
}
int addr(char*s)
{
	for(int i=0;i<tsize;i++)
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
int curr = 0;

void parse1(char* t)
{
	int i =0;
	while(OCT[i].op !=-1)
	{
		if(strcmp(t,OCT[i].name)==0)
		{
			csize+=OCT[i].size;
			return;
		}
		i++;
	}
	printf("Error: Opcode not Found!!");
	exit(1);
}

int parse2(char* t,unsigned char* c,int* op)
{
	int i=0;
	while(OCT[i].op != -1)
	{
		if(strcmp(t,OCT[i].name)==0)
		{
			if(OCT[i].op == INCLUDE)return 2;
			if(OCT[i].op == FUNC || OCT[i].op == LABEL) return 0;
			*c = OCT[i].op;
			*op = OCT[i].hasarg;
			return 1;
		}
		i++;
	}
	printf("Error: Opcode not Found!!");
	exit(1);
}



void runPass1(char* path)
{
	
	FILE *source = fopen(path,"r");

	if(!source)
	{
		perror("File Error!");return;
	}

	char line[256];
	while(fgets(line,sizeof(line),source))
	{
		char* c = strchr(line,';');
		if(c)*c='\0';;
		char* token = strtok(line," \n\t");
		if(!token)continue;
		else parse1(token);
		

		char *str = strtok(NULL,"\n\t");
        if(str){ str = trim(str);}
		if(strcmp(token,"INCLUDE")==0)
		{
			int found = 0;
			for(int i =0;i<pl;i++)
			{
				if(strcmp(libs[i],str)==0) {
					found = 1;
					break;
				}
			}
			if(!found)
			{
				strcpy(libs[pl++],str);
				runPass1(str);//asuming that we are in the same directory...
			}
		}
		if(str && str[0] == '"')
		{
			push(str);
			dsize+=strlen(str);
		}
		else if(str && (strcmp(token,"FUNC")==0 || strcmp(token,"LABEL") == 0))
		{
			insert(str,csize);
		}
				
	}
	fclose(source);
}


void runPass2(char* path,FILE* op)
{
	FILE *source = fopen(path,"r");

	if(!source || !op)
	{
		perror("File Error!");return;
	}
	
	char line[256];
	while(fgets(line,sizeof(line),source))
	{
		char* c = strchr(line,';');
		if(c)*c='\0';

		char* token = strtok(line," \n\t");
		if(!token)continue;

		unsigned char opcode = 0;
		int arg = 0;
		int has_arg = 0;
		
		int valid = parse2(token,&opcode,&has_arg);
     	if(valid == 1) fwrite(&opcode,sizeof(unsigned char),1,op);
     	else if(valid == 2)
     	{
     		char* arg_str = strtok(NULL,"\n\t");
            if(arg_str){ arg_str = trim(arg_str);}
			int found = 0;
			for(int i =0;i<pl;i++)
			{
				if(strcmp(libs[i],arg_str)==0) {
					found = 1;
					break;
				}
			}
			if(!found)
			{
				strcpy(libs[pl++],arg_str);
				runPass2(arg_str,op);
			}
			continue;
     	}
		else continue;
		
        if(has_arg)
        {
        	char* arg_str = strtok(NULL,"\n\t");
            if(arg_str) arg_str = trim(arg_str);

        	if(arg_str)
        	{
        		if(opcode == STORE || opcode== LOAD)
        		{
        			if(arg_str[0] >= '0' && arg_str[0] <= '9') arg = atoi(arg_str);
        			else arg = arg_str[0] - 'A';
        		}
        		else if(arg_str[0] == '\'')
        		{
        			arg = arg_str[1];
        		}
        		else if(arg_str[0] == '"')
        		{
        			arg = curr;
        			curr+=strlen(arg_str)-1;
        		}
        		else if(opcode == CALL || opcode == JMP || opcode == JE|| opcode == JNE || opcode == JL || opcode == JLE || opcode == JG|| opcode == JGE)
        		{
        			arg = addr(arg_str);
        			if(arg == -1) arg = atoi(arg_str);
        		}
        		else arg = atoi(arg_str);
        		
        	}
        	fwrite(&arg,sizeof(int),1,op);    
   	    }
   	    
	}
	fclose(source);
}


int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("Usage: ./");
		return 1;
	}
	char bfn[256];
	strcpy(bfn,argv[1]);
	char* dot = strrchr(bfn,'.');
	if(dot)strcpy(dot,".bin");
	else strcat(bfn,".bin");

	FILE *op = fopen(bfn,"wb");

	if(!op)
	{
		perror("File Error!");return 1;
	}
	
	runPass1(argv[1]);

	memset(libs,0,sizeof(libs));
	
	printf("Pass 1 Calculated Code Size: %d\n", csize);
	curr = csize;
	printf("Pass 2 Starting String Address at: %d\n",curr);
	runPass2(argv[1],op);
	printf("Pass 2 Starting String Address at: %d\n",curr);
	for(int i=0;i<=sp;i++)
	{
		char *s = strings[i];
		s++;
		fwrite(s,strlen(s)-1,1,op);
		char null = '\0';
		fwrite(&null,1,1,op);
	}	
	printf("Final Size: %d\n",csize);
	fclose(op);
	return 0;
}
