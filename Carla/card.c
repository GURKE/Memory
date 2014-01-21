#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "card.h"

int init_cards(struct Card(*cards)[], char FileName[]) // returns the amount of cards
{


}


int Save_Card(FILE *f, struct Card c);

struct sCard opencard(struct sCard ca)
{
	
	//difficulty
	FILE * f;
	f = fopen("./resources/Cards/Cards.txt");
	int difficulty = 0; 
	int chapter;
	int pair;
	char location[100];

	switch(c)
	{
		case '0':
	 	{
			ca.difficulty = easy;		// easy/ 0 ist ein integer (char wird ausgelesen, Bedeutung als int verarbeitet)
			break;
		}
		case '1':
		{
			ca.difficulty = medium;
			break;
		}
		case '2':
		{
			ca.difficulty = hard; 
			break; 
		}
		default:				// rekursiver Auruf 
		{		
			ca.difficulty = easy; 			//invalid difficulty was read, valid values: 0, 1, 2 
		}
	}

	ca.difficulty = c;
	return ca;
}