#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include "Object.h"
#include "Card.h"
#include "Picture.h"
#include "Button.h"

#pragma warning( disable : 4996 )

Sint16 x, y;
Uint16 type; // 0 = hard/background, 1 = card, 2 = Button
int enabled;

struct Picture picture;
struct Picture *back_picture;
struct Card card;
struct Button button;

int IS_NULL(struct Object o)
{
	if (o.type == 2) // Button
		return o.button.Picture->picture == NULL ? 1 : 0;
	else
		return o.picture.picture == NULL ? 1 : 0;
}

SDL_Rect *Create_Rect_BO(struct Object o, int x, int y)
{
	SDL_Rect *rect = (SDL_Rect *)malloc(sizeof(SDL_Rect));
	(*rect).h = o.picture.picture->h;
	(*rect).w = o.picture.picture->w;
	(*rect).x = o.x + x;
	(*rect).y = o.y + y;
	return rect;
}