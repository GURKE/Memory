#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Card.h"

#pragma warning( disable : 4996 )


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
	while (fscanf(f, "%s %d %d", &c, &(*cards)[j].difficulty, &(*cards)[j].type) != EOF) //End of File =  EOF
	{
		*(*cards)[j].picture = load_picture(*(*cards)[j].picture, c);

		if ((*cards)[j].picture->picture == NULL)
			return FAILED_LOADING_IMAGE;

		(*cards)[j].visible = 0;
		j++;
	}

	return 0;
}

int Save_Cards(struct Card cards[], char FileName[])
{

}