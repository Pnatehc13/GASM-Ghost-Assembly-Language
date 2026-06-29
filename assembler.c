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
int vp = 0;
int dp = 0;
typedef struct 
{
    char name[64];
    int address;
}GlobalVar;
GlobalVar global_table[256];
int gtp = 0;

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

int get_global_addr(char* s)
{
	if(s[0] == '[' && s[strlen(s)-1] == ']')
	{
		char varname[64];
		int len = strlen(s) - 2;
		strncpy(varname, s + 1, len);
    varname[len] = '\0';
    for(int i =0;i<gtp;i++)
    {
			if(strcmp(global_table[i].name ,varname) == 0)
			{
				return global_table[i].address;
			}
    }
	}
	return -1;
}


int get_or_add_string(char* s)
{
	char clean[256];
	int j = 0;
	for(int i = 1; i < (int)strlen(s) - 1; i++)
	{
		 if(s[i] == '\\' && s[i+1] == 'n')
		 {
				clean[j++] = '\n';
				i++;
		 }
		 else clean[j++] = s[i];
	}
	clean[j] = '\0';
	
	int offset = 0;
	while(offset < shared_string_ptr) 
	{
		if(strcmp(&shared_string_table[offset], clean) == 0) 
		{
			return DATA_START + offset; 
		}
		offset += strlen(&shared_string_table[offset]) + 1;
	}
	int new_addr = DATA_START + shared_string_ptr;
	strcpy(&shared_string_table[shared_string_ptr], clean);
	shared_string_ptr += strlen(clean) + 1;
	return new_addr;
}



int csize = 0;
int dsize = 0;
int curr = 0;
int current_global_offset = 0;

void parse1(char* t)
{
	if(strncmp(t,"global_",strlen("global_")) == 0)
	{
		strcpy(global_table[gtp].name,t);
		char* size_token = strtok(NULL, " \n\t");	
		int element_count = size_token ? atoi(size_token) : 1;
		current_global_offset += (4 * element_count);
		global_table[gtp].address = GLOBALS_TOP - current_global_offset;
		gtp++;
		csize+=0;
		return;
	}
	else if(t[0] == '[')
	{
		csize+=4;
		return;
	}
	int i =0;
	while(OCT[i].op !=-1)
	{
		if(strcmp(t,OCT[i].name)==0)
		{
			csize+=(OCT[i].size*4);
			return;
		}
		i++;
	}
	printf("Error: Opcode not Found!!");
	exit(1);
}

int parse2(char* t,unsigned char* c,int* op)
{
	if(strncmp(t, "global_", 7) == 0) return 0;
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
     	if(valid == 1)
     	{
					int op_word = (int)opcode;
					fwrite(&op_word,sizeof(int),1,op);
     	}
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
						int globaladdr = get_global_addr(arg_str);
						if(globaladdr !=-1)
						{
							arg = globaladdr;
						}
        		else if(opcode == STORE || opcode== LOAD)
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
        			arg = get_or_add_string(arg_str) ;
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


int assm_start(char* path)
{
	char bfn[256];
	strcpy(bfn,path);
	char* dot = strrchr(bfn,'.');
	if(dot)strcpy(dot,".bin");
	else strcat(bfn,".bin");

	FILE *op = fopen(bfn,"wb");

	if(!op)
	{
		perror("File Error!");return 1;
	}
	
	runPass1(path);

	memset(libs,0,sizeof(libs));
	
	printf("Pass 1 Calculated Code Size: %d\n", csize);
	curr = csize;
	printf("Pass 2 Starting String Address at: %d\n",curr);
	runPass2(path,op);
	printf("Final Size: %d\n",csize);
	fclose(op);
	return 0;
}
