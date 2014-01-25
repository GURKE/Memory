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
#include "Label.h"

#pragma warning( disable : 4996 )

Sint16 x, y;
Uint16 type; // 0 = hard/background, 1 = card, 2 = Button
int enabled;

struct Object O_New_Button(struct Object O, char Text[], int Button_Type, int Value, int X, int Y);
struct Object O_New_Label(struct Object O, char Text[], int X, int Y);
struct Object O_New_Object(struct Object O, int X, int Y, int Type, char Filename1[], char Filename2[], int ButtonType, int ButtonValue);
	
struct Picture picture;
struct Picture *back_picture;
struct Card card;
struct Button button;
struct Label label;

int IS_NULL(struct Object o)
{
	if (o.type == 2) // Button
		return o.button.Picture.picture == NULL ? 1 : 0;
	else
		return o.picture.picture == NULL ? 1 : 0;
}

SDL_Rect *Create_Rect_BO(struct Object *o, int move) // Creates Rectangle of picture for the paint procedure
{
	SDL_Rect *rect = (SDL_Rect *)malloc(sizeof(SDL_Rect));
	(*rect).h = o->picture.picture->h;
	(*rect).w = o->picture.picture->w;
	(*rect).x = o->x - (move ? (o->button.Clicked_Picture.picture->w - o->button.Picture.picture->w) / 2 : 0);
	(*rect).y = o->y - (move ? (o->button.Clicked_Picture.picture->h - o->button.Picture.picture->h) / 2 : 0);
	return rect;
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

struct Object O_New_Button(struct Object O, char Text[], int Button_Type, int Value, int X, int Y)
{
	O.button = New_Button(O.button, Text, Button_Type);
	O.enabled = 1;
	O.picture = O.button.Picture;
	O.type = TButton;
	O.button.Value = Value;
	O.x = X;
	O.y = Y;
	return O;
}

struct Object O_New_Label(struct Object O, char Text[], int X, int Y)
{
	O.x = X;
	O.y = Y;
	strcpy(O.label.Text, Text);
	O.enabled = 1;
	O.picture = Create_Picture_By_Text(O.picture, O.label.Text, 0);
	O.type = TLabel;
	return O;
}

struct Object O_New_Object(struct Object O, int X, int Y, int Type, char Filename1[], char Filename2[], int ButtonType, int ButtonValue)
{
	O.x = X;
	O.y = Y;

	if (Type == TButton)
	{
		O.button.Picture = load_picture(O.button.Picture, Filename1);
		O.picture = O.button.Picture;
		O.button.Clicked_Picture = load_picture(O.button.Clicked_Picture, Filename2);
	}
	else
		O.picture = load_picture(O.picture, Filename1);
	
	O.button.Type = ButtonType;
	O.button.Value = ButtonValue;
	O.enabled = 1;
	O.type = Type;
	
	return O;
}