#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Object.h"

#pragma warning( disable : 4996 )

struct Player
{
	char Name[20];
	struct Object *ObPlayername;
	struct Object *ObPlayerPoints;
	int FoundPairs;
};

#endif