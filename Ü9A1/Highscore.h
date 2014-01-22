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

char* gethighscore();
void writehighscore(char name[], int moves);
struct Object *GetHighscoreItems();

#endif