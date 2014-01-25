#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Generel_Proc.h"

int freadString(FILE *f, char(*Output)[], char seperator); // Returns 0 for ok, -1 for string is too long
char* concat(char *s1, char *s2);

char* concat(char *s1, char *s2)
{
	char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);//+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);

	return result;
}

int freadString(FILE *f, char(*Output)[], char seperator, int MaxLength) // Returns 0 for ok, -1 for string is too long
{
	int i = 0;
	while (((*Output)[i] = fgetc(f)) != seperator && (*Output)[i] != '\0')
	{
		if (i == MaxLength - 1) return -1; else i++;
	}
	(*Output)[i] = '\0';
	return 0;
}