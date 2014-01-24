#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Card.h"

#pragma warning( disable : 4996 )


int freadString(FILE *f, char(*Output)[], char seperator); // Returns 0 for ok, -1 for string is too long

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

int init_cards(struct Card (*cards)[], char FileName[])
{	
	FILE *f;
	f = fopen(FileName, "r");

	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		/*int *x; BSP für malloc
		x = (int *)malloc(sizeof(int));*/
		(*cards)[i].picture = (struct Picture *)malloc(sizeof(struct Picture));
		(*cards)[i].picture->picture = NULL;
	}

	int j = 0;
	char c[100]; //string für den Dateipfad
	while (1)
	{
		if (freadString(f, &c, '#', 100))
			break;
		if (fscanf(f, "%d#%d#\n", &(*cards)[j].difficulty, &(*cards)[j].type) == EOF) //End of File =  EOF
			break;

		*(*cards)[j].picture = load_picture(*(*cards)[j].picture, c);
		

		if ((*cards)[j].picture->picture == NULL)
			return FAILED_LOADING_IMAGE;

		(*cards)[j].visible = 0;
		j++;
	}

	return 0;
}

int Save_Card(FILE *f, struct Card c)
{
	printf(f, "%d %d %d", c.difficulty, c.type, c.visible);
	return 0;
}