#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Objectmanager.h"
#include "Object.h"
#include "Card.h"
#include "Picture.h"
#include "Button.h"
#include "Label.h"

struct Object objects[Number_Of_Menues][ARRAY_LENGTH];
int Buttons[Number_Of_Menues][ARRAY_LENGTH];
int NumberOfButtons[Number_Of_Menues];

int paint_screen(SDL_Surface *_screen);
int dist2object(int x, int y, int type[], int AmOfTypes);
struct Objectmanager Load_Objects(struct Objectmanager oman, char Filename[]);

int Akt_Menu = 0;
int Akt_Button = -1;

struct Objectmanager Load_Objects(struct Objectmanager oman, char Filename[])
{
	FILE *f;
	f = fopen(Filename, "r");

	int AM = 0;
	while (fscanf(f, "%d", &AM) != EOF)
	{
		int i = 0;
		// Load the buttons
		char c[50];
		fscanf(f, "%s", &c);
		if (c[0] == 'B')
		{
			fscanf(f, "%d", &NumberOfButtons[AM]);

			for (int j = 0; j < NumberOfButtons[AM]; j++)
			{
				int x = 0, y = 0, Type = 0;
				if (fscanf(f, "%d %d %s %d", &x, &y, &c, &Type) == EOF)
					return oman;

				oman.objects[AM][i] = O_New_Button(oman.objects[AM][i], c, Type, x, y);
				Buttons[AM][j] = i;

				i++;
			}
			fscanf(f, "%s", &c);
		}

		if (c[0] == 'O')
		{
			int NumberOfObjects = 0;
			fscanf(f, "%d", &NumberOfObjects);
			for (int j = 0; j < NumberOfObjects; j++)
			{
				if (fscanf(f, "%s %d %d %d", &c, &oman.objects[AM][i].x, &oman.objects[AM][i].y, &oman.objects[AM][i].type) == EOF)
					return oman;

				oman.objects[AM][i].picture = load_picture(oman.objects[AM][i].picture, c);
				if (oman.objects[AM][i].picture.picture == NULL)
					return oman;

				oman.objects[AM][i].enabled = 1;
				i++;
			}
			fscanf(f, "%s", &c);
		}

		if (c[0] == 'L')
		{
			int NumberOfLabel = 0;
			fscanf(f, "%d", &NumberOfLabel);
			for (int j = 0; j < NumberOfLabel; j++)
			{
				if (fscanf(f, "%d %d %s", &oman.objects[AM][i].x, &oman.objects[AM][i].y, &oman.objects[AM][i].label.Text) == EOF)
					return oman;

				oman.objects[AM][i].picture = Create_Picture_By_Text(oman.objects[AM][i].picture, oman.objects[AM][i].label.Text, 0);
				if (oman.objects[AM][i].picture.picture == NULL)
					return oman;

				oman.objects[AM][i].enabled = 1;
				i++;
			}
		}
	}

	return oman;
}

int paint_screen(SDL_Surface *_screen)
{
	_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	SDL_FillRect(_screen, NULL, _bg_color);

	int i = 0;
	while (!IS_NULL(objects[Akt_Menu][i]))
	{
		if (objects[Akt_Menu][i].enabled)
		{
			switch (objects[Akt_Menu][i].type)
			{
			case 1: // card
				if (objects[Akt_Menu][i].card.visible)
					SDL_BlitSurface(objects[Akt_Menu][i].picture.picture, NULL, _screen, Create_Rect_BO(&objects[Akt_Menu][i], 0)); // Foreground of the card
				else
					SDL_BlitSurface(Card_Background.picture, NULL, _screen, Create_Rect_BO(&objects[Akt_Menu][i], 0)); // Background of the card
				break;
			case 2: // Button
				if (objects[Akt_Menu][i].button.Clicked)
					SDL_BlitSurface(objects[Akt_Menu][i].button.Clicked_Picture.picture, NULL, _screen, Create_Rect_BO(&objects[Akt_Menu][i], 0)); // Draw a clicked button
				else
					SDL_BlitSurface(objects[Akt_Menu][i].button.Picture.picture, NULL, _screen, Create_Rect_BO(&objects[Akt_Menu][i], 0)); // Foreground of the card

				break;
			default:
				SDL_BlitSurface(objects[Akt_Menu][i].picture.picture, NULL, _screen, Create_Rect_BO(&objects[Akt_Menu][i], 0)); // Draws everything else
				break;
			}
		}
		i++;
	}

	SDL_Flip(_screen);

	return 0;
}

int dist2object(int x, int y, int type[], int AmOfTypes)
{
	int i = 0;
	while (!IS_NULL(objects[Akt_Menu][i]))
	{
		for (int j = 0; j < AmOfTypes; j++)
		{
			if (objects[Akt_Menu][i].enabled && !objects[Akt_Menu][i].card.visible && objects[Akt_Menu][i].type == type[j])
			{
				double xrel = x - objects[Akt_Menu][i].x;
				double yrel = y - objects[Akt_Menu][i].y;
				if (xrel > 0 && xrel < (objects[Akt_Menu][i].picture).picture->w)
				if (yrel > 0 && yrel < (objects[Akt_Menu][i].picture).picture->h)
					return i;
			}
		}
		i++;
	}
	return -1;
}