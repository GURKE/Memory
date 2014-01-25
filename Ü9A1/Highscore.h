#ifndef _HIGHSCORE_H_
#define _HIGHSCORE_H_

#pragma warning( disable : 4996 )
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include "Highscoreitem.h"
#include "Object.h"

#pragma warning( disable : 4996 )

#define HS_LENGTH 10

char* Load_Highscore(int AmPlayers);
int testhighscore(int moves, int AmPlayers);
struct Object *GetHighscoreItems(int AmPlayers);
void add_Highscoreitems(struct Player Players[8], int AmPlayers, int AmCards);                 // void: nichts zurückgeben, wert soll eingetragen werden
print_Highscore(int AmPlayers);

#endif