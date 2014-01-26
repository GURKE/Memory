#ifndef _CARDS_H_
#define _CARDS_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Generel_Proc.h"
#include "Picture.h"

struct Pair
{
	int id;
	int difficulty;
	int topic;
	int visibility;
	struct Picture *picture1;
	struct Picture *picture2;
};

struct Pair* ReadDeck(struct Pair* arraytofill, char* addresstoload);

#endif
