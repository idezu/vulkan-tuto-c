#include <stdio.h>
#include <stdlib.h>

int readFileB(const char *fileName, char ** fileContent)
	{
		FILE*  file = fopen(fileName,"r");
		fseek(file,0,SEEK_END);
		int size = ftell(file);
		fseek(file,0 , SEEK_SET);
		*fileContent = malloc(size);
		if (*fileContent == NULL)
			{
				return -1;
			}
		return fread(*fileContent,1,size,file);
	}
