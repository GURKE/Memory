#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Card.h"
#include "Picture.h"
#include "Button.h"

#pragma warning( disable : 4996 )

#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   1024
#define SCREEN_BPP      32

Uint32 _bg_color;
struct Picture Card_Background;

#define THard 0
#define TCard 1
#define TButton 2

struct Object
{
	int x, y;
	int type; // 0 = hard/background, 1 = card, 2 = button
	int enabled;

	struct Picture picture;
	struct Card card;
	struct Button button;
};

int IS_NULL(struct Object o);
SDL_Rect *Create_Rect_BO(struct Object *o, int move);
int dist2object(struct Object *_objects[], int x, int y, int type[], int AmOfTypes);
int paint_screen(SDL_Surface *_screen, struct Object(*_objects)[]);
int Save_Objects(struct Object objects[], FILE *f);

#endif