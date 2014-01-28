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
#include "Player.h"
#include "Generel_Proc.h"

#pragma warning( disable : 4996 )

/*Laufvariable z für Züge nötig, die bei jedem Zug um eins erhöht wird, in der Highscoreliste wird dabei das
kleinste Ergebnis aufsteigend (und der dazugehörige Spieler) gespeichert Julian*/

/*int anzahl;												// Anzahl der Einträge/Highscores in der Datei
int moves;												// erreichte Züge des Benutzers
char user[NAME_LENGTH];									// Name des Benutzers*/


struct Highscoreitem Highscoreitems[8][HS_LENGTH];

char* Load_Highscore(int AmPlayers);
int testhighscore(int moves, int AmPlayers);
struct Object *GetHighscoreItems(int AmPlayers); 
void add_Highscoreitems(struct Player Players[8], int AmPlayers, int AmCards);                    // void: nichts zurückgeben, wert soll eingetragen werden
print_Highscore(int AmPlayers);

char* Load_Highscore(int AmPlayers)
{
	char c2[ARRAY_LENGTH];
	c2[0] = '\0';
	char c[2];
	sprintf(&c, "%d", AmPlayers);
	strcat(&c2, &"./resources/Highscores/highscore_");
	strcat(&c2, &c);
	strcat(&c2, &".txt");
	FILE* liste = fopen(c2, "r");					// Öffnen der Textdatei
	
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
				Highscoreitems[AmPlayers - 1][ranking].ranking = ranking;
				break;
			case 1:
				strcpy(Highscoreitems[AmPlayers - 1][ranking].name, Zwischenspeicher);
				break;
			case 2:
				Highscoreitems[AmPlayers - 1][ranking].moves = atoi(Zwischenspeicher);
				break;
			}

			underscore++;
			Zwischenspeicher[0] = '\0';
		}
		else if (c[0] == '\n')									// nächster Rang/Zeile
		{
			underscore = 0;
			ranking++;
			Zwischenspeicher[0] = '\0';
		}
		else
		{													// Zeichen einlesen
			strncat(Zwischenspeicher, c, 1);
		}

	} while (c[0] != EOF && ranking < 10);

	return Highscoreitems[AmPlayers - 1];
}

int testhighscore(int moves, int AmPlayers)                                // moves Anzahl der Züge
{
	int ranking = 0;
	if (AmPlayers > 1)
		while (Highscoreitems[AmPlayers - 1][ranking].moves > moves) ranking++; // string wird in integer umgewandelt
	else
	while (Highscoreitems[AmPlayers - 1][ranking].moves < moves && Highscoreitems[AmPlayers - 1][ranking].name[0] != '\0') ranking++; // string wird in integer umgewandelt

	if (ranking < 10)
		return ranking;
	else
		return -1;
}

void add_Highscoreitems(struct Player Players[8], int AmPlayers, int AmCards)							// void: nichts zurückgeben, wert soll eingetragen werden
{
	int i = 0;
	for (; i < AmPlayers; i++)
	{
		if (AmPlayers > 1)
			Players[i].FoundPairs = ((2. / 5) * (Players[i].FoundPairs - 5) * (Players[i].FoundPairs - 5) * (Players[i].FoundPairs - 5) + 50) * AmCards / 20;					// (2/5*(Pairs-5)^3+50)*Cards/MaxCards
		else
			Players[i].FoundPairs = CalcPoints((double)Players[i].FoundPairs, (double)AmCards);// (100 - ((2. / 5) * (Players[i].FoundPairs - 5) * (Players[i].FoundPairs - 5) * (Players[i].FoundPairs - 5) + 50)) * AmCards / 20;					// (2/5*(Pairs-5)^3+50)*Cards/MaxCards

		int ranking = testhighscore(Players[i].FoundPairs, AmPlayers);						// ranking ist zeile aus testhighscore 
		if (ranking > -1)																	// wenn 0 wird nicht ausgeführt
		{
			int j = 9;
			while (j > ranking)
			{
				Highscoreitems[AmPlayers - 1][j] = Highscoreitems[AmPlayers - 1][j-- - 1];
			}
			strcpy(Highscoreitems[AmPlayers - 1][ranking].name, Players[i].Name);
			Highscoreitems[AmPlayers - 1][ranking].moves = Players[i].FoundPairs;
			Highscoreitems[AmPlayers - 1][ranking].ranking = ranking;
		}
	}

	print_Highscore(AmPlayers);
}

int CalcPoints(double x, double y)
{
	return 100 * x / y;
	double PointsForMoves = ((sqrt(sqrt((int)((-x - 10) / 10) ^ 2))*((-x - 10) / 10) / sqrt((int)((-x - 10) / 10) ^ 2)) / 2 + 0.5);
	double PointsForCards = ((sqrt(sqrt((int)((y - 10) / 10) ^ 2))*((y - 10) / 10) / sqrt((int)((y - 10) / 10) ^ 2)) / 2 + 0.5);
	return (int)(100 * PointsForCards * PointsForMoves);
}

print_Highscore(int AmPlayers)
{
	char c[2];
	sprintf(&c, "%d", AmPlayers);
	FILE *f = fopen(concat(concat("./resources/Highscores/highscore_", c), ".txt"), "w");					// Öffnen der Textdatei

	for (int i = 0; i < HS_LENGTH; i++)
	{
		fprintf(f, "%d_%s_%d_\n", Highscoreitems[AmPlayers - 1][i].ranking, Highscoreitems[AmPlayers - 1][i].name, Highscoreitems[AmPlayers - 1][i].moves);
	}
	fclose(f);
}

struct Object *GetHighscoreItems(int AmPlayers)
{
	for (int i = 0; i < 10; i++)
	{
		Highscoreitems[i]->moves = -1;
		Highscoreitems[i]->name[0] = '\0';
		Highscoreitems[i]->ranking = -1;
	}

	Load_Highscore(AmPlayers);

	struct Object *o = (struct Object*)malloc(2 * HS_LENGTH * sizeof(struct Object));
	for (int i = 0; i < HS_LENGTH; i++)
	{
		*o = O_New_Label(*o, Highscoreitems[AmPlayers - 1][i].name, 364, 181 + 49 * i);
		o->type = THighscoreitem;
		o++;
		char c[10];
		sprintf(c, "%d", Highscoreitems[AmPlayers - 1][i].moves);

		*o = O_New_Label(*o, c, 845, 181 + 49 * i);
		o->type = THighscoreitem;
		o++;
	}
	for (int i = 0; i < HS_LENGTH * 2; i++) o--;
	return o;
}