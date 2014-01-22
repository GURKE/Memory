#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include "Highscoreitem.h"
#include "Object.h"
#include "Highscore.h"
#pragma warning( disable : 4996 )

/*Laufvariable z für Züge nötig, die bei jedem Zug um eins erhöht wird, in der Highscoreliste wird dabei das
kleinste Ergebnis aufsteigend (und der dazugehörige Spieler) gespeichert Julian*/

/*int anzahl;												// Anzahl der Einträge/Highscores in der Datei
int moves;												// erreichte Züge des Benutzers
char user[NAME_LENGTH];									// Name des Benutzers*/


struct Highscoreitem Highscoreitems[HS_LENGTH];

char* gethighscore();
void writehighscore(char name[], int moves);
int testhighscore(int moves);
struct Object *GetHighscoreItems();

char* gethighscore()
{
	FILE* liste = fopen("./resources/highscore.txt", "r");                   // Öffnen der Textdatei
	char c[1];                                                 // Zeichen
	int underscore = 0;
	int ranking = 0;

	char Zwischenspeicher[NAME_LENGTH];
	Zwischenspeicher[0] = '\0';

	do
	{
		c[0] = fgetc(liste);
		if (c[0] == '_')										// nach Unterstrich nächste Eigenschaft
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
		else if (c[0] == '\n')									// nächster Rang/Zeile
		{
			underscore = 0;
			ranking++;
		}
		else
		{													// Zeichen einlesen
			strncat(Zwischenspeicher, c, 1);
		}

	} while (c != EOF && ranking < 10);

	return Highscoreitems;
}

int testhighscore(int moves)                                // moves Anzahl der Züge
{
    int ranking = 0;
	while (Highscoreitems[ranking].moves < moves) ranking++;             // string wird in integer umgewandelt
	if (ranking < 10)
		return ranking - 1;
	else
		return -1;
}


void writehighscore(char name[], int moves)                    // void: nichts zurückgeben, wert soll eingetragen werden
{
	int ranking = testhighscore(moves);						// ranking ist zeile aus testhighscore 
	if (ranking > -1)										// wenn 0 wird nicht ausgeführt
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

	print_Highscore();
}

print_Highscore()
{
	FILE *f = fopen("./resources/highscore.txt", "w");
	for (int i = 0; i < HS_LENGTH; i++)
	{
		fprintf(f, "%d_%s_%d_\n", Highscoreitems[i].ranking, Highscoreitems[i].name, Highscoreitems[i].moves);
	}
}

struct Object *GetHighscoreItems()
{
	gethighscore();

	struct Object *o = (struct Object*)malloc(2 * HS_LENGTH * sizeof(struct Object));
	for (int i = 0; i < HS_LENGTH; i++)
	{
		*o = O_New_Label(*o, Highscoreitems[i].name, 364, 181 + 49 * i);
		o->type = THighscoreitem;
		o++;
		char c[3];
		sprintf(c, "%d", Highscoreitems[i].moves);

		*o = O_New_Label(*o, c, 845, 181 + 49 * i);
		o->type = THighscoreitem;
		o++;
	}
	for (int i = 0; i < HS_LENGTH * 2; i++) o--;
	return o;
}