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

int paint_screen(SDL_Surface *_screen, struct Object(*_objects)[]);
int dist2object(struct Object (*_objects)[], int x, int y, int type[], int AmOfTypes);

struct Picture picture;
struct Picture *back_picture;
struct Card card;
struct Button button;

int IS_NULL(struct Object o)
{
	if (o.type == 2) // Button
		return o.button.Picture.picture == NULL ? 1 : 0;
	else
		return o.picture.picture == NULL ? 1 : 0;
}

SDL_Rect *Create_Rect_BO(struct Object *o) // Creates Rectangle of picture for the paint procedure
{
	SDL_Rect *rect = (SDL_Rect *)malloc(sizeof(SDL_Rect));
	(*rect).h = o->picture.picture->h;
	(*rect).w = o->picture.picture->w;
	(*rect).x = o->x;
	(*rect).y = o->y;
	return rect;
}

int paint_screen(SDL_Surface *_screen, struct Object (*_objects)[])
{
	_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	SDL_FillRect(_screen, NULL, _bg_color);

	int i = 0;
	while (!IS_NULL((*_objects)[i]))
	{
		if ((*_objects)[i].enabled)
		{
			switch ((*_objects)[i].type)
			{
			case 1: // card
				if ((*_objects)[i].card.visible)
					SDL_BlitSurface((*_objects)[i].picture.picture, NULL, _screen, Create_Rect_BO(&(*_objects)[i], 0)); // Foreground of the card
				else
					SDL_BlitSurface(Card_Background.picture, NULL, _screen, Create_Rect_BO(&(*_objects)[i], 0)); // Background of the card
				break;
			case 2: // Button
				if ((*_objects)[i].button.Clicked)
					SDL_BlitSurface((*_objects)[i].button.Clicked_Picture.picture, NULL, _screen, Create_Rect_BO(&(*_objects)[i], 0)); // Draw a clicked button
				else
					SDL_BlitSurface((*_objects)[i].button.Picture.picture, NULL, _screen, Create_Rect_BO(&(*_objects)[i], 0)); // Foreground of the card

				break;
			default:
				SDL_BlitSurface((*_objects)[i].picture.picture, NULL, _screen, Create_Rect_BO(&(*_objects)[i], 0)); // Draws everything else
				break;
			}
		}
		i++;
	}

	SDL_Flip(_screen);

	return 0;
}

int dist2object(struct Object (*_objects)[], int x, int y, int type[], int AmOfTypes)
{
	int i = 0;
	while (!IS_NULL((*_objects)[i]))
	{
		for (int j = 0; j < AmOfTypes; j++)
		{
			if ((*_objects)[i].enabled && !(*_objects)[i].card.visible && (*_objects)[i].type == type[j])
			{
				double xrel = x - (*_objects)[i].x;
				double yrel = y - (*_objects)[i].y;
				if (xrel > 0 && xrel < ((*_objects)[i].picture).picture->w)
				if (yrel > 0 && yrel < ((*_objects)[i].picture).picture->h)
					return i;
			}
		}
		i++;
	}
	return -1;
}

int Save_Objects(struct Object objects[], FILE *f)
{
	int i;
	for (i = 0; !IS_NULL(objects[i]); i++)
	{
		fprintf(f, "%d %d %d %d", objects[i].x, objects[i].y, objects[i].enabled, objects[i].type);
		Save_Picture(f, objects[i].picture);

		switch (objects[i].type)
		{
		case THard:
			break;
		case TCard:
			Save_Card(f, objects[i].card);
			break;
		case TButton:
			Save_Button(f, objects[i].button);
			break;
		default:
			break;
		}
	}
}
