#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Card.h"
#include "Picture.h"
#include "Button.h"

#pragma warning( disable : 4996 )

struct Object
{
	Sint16 x, y;
	Uint16 type; // 0 = hard/background, 1 = card
	int enabled;

	struct Picture picture;
	struct Card card;
	struct Button button;
};

int IS_NULL(struct Object o);
SDL_Rect *Create_Rect_BO(struct Object o, int x, int y);

#endif