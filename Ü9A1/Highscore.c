#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include "Highscoreitem.h"

#pragma warning( disable : 4996 )

/*Laufvariable z f�r Z�ge n�tig, die bei jedem Zug um eins erh�ht wird, in der Highscoreliste wird dabei das
kleinste Ergebnis aufsteigend (und der dazugeh�rige Spieler) gespeichert Julian*/

/*int anzahl;												// Anzahl der Eintr�ge/Highscores in der Datei
int moves;												// erreichte Z�ge des Benutzers
char user[NAME_LENGTH];									// Name des Benutzers*/


struct Highscoreitem Highscoreitems[HS_LENGTH];


char* gethighscore()
{
	FILE* liste = fopen("high.txt", "r");                   // �ffnen der Textdatei
	char c;                                                 // Zeichen
	int underscore = 0;
	int ranking = 0;

	char Zwischenspeicher[NAME_LENGTH];
	Zwischenspeicher[0] = '\0';

	do
	{
		c = fgetc(liste);
		if (c == '_')										// nach Unterstrich n�chste Eigenschaft
		{
			switch (underscore)
			{
			case 0:
				Highscoreitems[ranking].ranking = ranking;
				break;
			case 1:
				strcpy(Highscoreitems[ranking].name, Zwischenspeicher);				
				break;
			case 2:
				Highscoreitems[ranking].moves = atoi(Zwischenspeicher);
				break;
			}

			underscore++;
			Zwischenspeicher[0] = '\0';
		}
		else if (c == '\n')									// n�chster Rang/Zeile
		{
			underscore = 0;
			ranking++;
		}
		else
		{													// Zeichen einlesen
			strcat(Zwischenspeicher, c);
		}

	} while (c != EOF && ranking < 10);

	return Highscoreitems;
}

int testhighscore(int moves)                                // moves Anzahl der Z�ge
{
    int ranking = 0;
	while (Highscoreitems[ranking].moves < moves) ranking++;             // string wird in integer umgewandelt
	if (ranking < 10)
		return ranking - 1;
	else
		return -1;
}


void writehighscore(char name[], int moves)                    // void: nichts zur�ckgeben, wert soll eingetragen werden
{
	int ranking = testhighscore(moves);						// ranking ist zeile aus testhighscore 
	if (ranking > -1)										// wenn 0 wird nicht ausgef�hrt
	{
		int i = 9;
		while (i > ranking)
		{
			Highscoreitems[i] = Highscoreitems[i-- - 1];
		}
		strcpy(Highscoreitems[ranking].name, name);
		Highscoreitems[ranking].moves = moves;
		Highscoreitems[ranking].ranking = ranking;

	}
}