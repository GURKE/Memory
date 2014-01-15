#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "card.h"




struct sCard opencard(struct sCard ca)
{
	
	//difficulty
	FILE * f;
	f = fopen("./resources/Cards/Cards.txt");
	int difficulty = 0; 
	int chapter;
	int pair;
	char location[100];

	/*switch(c)
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
	}*/

	ca.difficulty = c;
	return ca;
}



	

	Daten speicherst,  immer als string von 8-Bit Variablen ausgelesem,
	 8-Bit Variablen entsprehcne immer ASCII-Char
	Datei difficulty.txt auslesem, enthält nur ein Zeichen: '1'
	FILE* reader = fopen("*\difficulty.txt");


