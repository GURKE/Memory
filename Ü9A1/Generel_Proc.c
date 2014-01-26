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
int IsIn(char Text[], char Texttofind[]);

char* concat(char *s1, char *s2)
{
	char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);//+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);

	return result;
}

int freadString(FILE *f, char(*Output)[], char seperator[], int MaxLength) // Returns position of seperator for ok, -1 for string is too long
{
	int i = 0; int j = 0;

	do
	{
		(*Output)[i] = fgetc(f);
		j = 0;
		while ((*Output)[i] != seperator[j] && seperator[j] != '\0') j++;
		if ((*Output)[i] == seperator[j])
		{
			(*Output)[i] = '\0';
			return j;
		}
		i++;
	} while ((*Output)[i - 1] != '\0' && i < MaxLength);
	return -1;
}

int IsIn(char Text[], char Texttofind[])
{
	int i = 0;
	for (; Text[i] != '\0'; i++)
	{
		int j = 0;
		while (Text[i] == Texttofind[j])
		{
			i++; j++;
			if (Texttofind[j] == '\0')
				return 1;
		}
	}

	return 0;
}