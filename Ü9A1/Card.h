#ifndef _CARDS_H_
#define _CARDS_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>
#inlcude "General_Proc.h"

struct Pair{
	int id;
	int difficulty;
	int topic;
	char location[ARRAY_LENGTH];
	int visibility;
	SDL_Surface *picture;
};

#endif
