#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Card.h"
#include "Generel_Proc.h"
#include "Pair.h"

#pragma warning( disable : 4996 )


int init_cards_old(struct Card (*cards)[], char FileName[])
{	
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		(*cards)[i].picture = (struct Picture *)malloc(sizeof(struct Picture));
		(*cards)[i].picture->picture = NULL;
	}

	struct Pair pairs[ARRAY_LENGTH];
	ReadDeck(&pairs, FileName);

	int j = 0;
	char c[100]; //string für den Dateipfad
	while (1)
	{
		//*(*cards)[j].

//		if (freadString(f, &c, "#", 100))
			break;
	//	if (fscanf(f, "%d#%d#\n", &(*cards)[j].difficulty, &(*cards)[j].type) == EOF) //End of File =  EOF
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