#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include <memory.h>

#ifndef _CARD_H_

#include "Picture.h"

#define easy 0
#define medium 1
#define hard 2

struct sCard
{
	int difficulty;			/* 0 = easy, 1 = medium, 3 = hard */
	int visibility;			/* 0 = covered => show backside or 1 = uncovered => show picture */
	int chapter;			/* 0 = chapter 1, 1 = chapter 2, 2 = chapter 3 */
	int pair;			/* 1-1 => Pair 1, 2-2 => Pair 2 ... 60-60 => Pair 60 */
	char location[100];		/* Dateipfad */		
}

#endif
