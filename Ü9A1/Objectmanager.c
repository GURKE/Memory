#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Objectmanager.h"
#include "Object.h"
//#include "Card.h"
#include "Picture.h"
#include "Button.h"
#include "Label.h"

struct Objectmanager Load_Objects(struct Objectmanager oman, char Filename[]);
int paint_screen(SDL_Surface *_screen, struct Object objects[]);
int dist2object(struct Object objects[], int x, int y, int type[], int AmOfTypes);
int Change_Menu(struct Objectmanager *oman, SDL_Surface *_screen, int Menu, int *Akt_Button);
struct Objectmanager init_cards(struct Objectmanager *oman, struct Pair(*cards)[], char FileName[], struct Picture Background, int Menu);


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
			fscanf(f, "%d", &oman.NumberOfButtons[AM]);

			for (int j = 0; j < oman.NumberOfButtons[AM]; j++)
			{
				int x = 0, y = 0, Type = 0, Value = 0;
				
				if (fscanf(f, "%d#%d#%d#%d#", &x, &y, &Type, &Value) == EOF)
					return oman;
				if (freadString(f, &c, "#", 50) < 0)
					return oman;

				oman.objects[AM][i] = O_New_Button(oman.objects[AM][i], c, Type, Value, x, y);
				oman.Buttons[AM][j] = i;
				i++;
			}
			fscanf(f, "%s", &c);
		}

		if (c[0] == 'O')
		{
			int NumberOfObjects = 0;
			fscanf(f, "%d\n", &NumberOfObjects);
			for (int j = 0; j < NumberOfObjects; j++)
			{
				if (freadString(f, &c, "#", 50) < 0)
					return oman;
				if (fscanf(f, "%d#%d#%d#%d#%d#", &oman.objects[AM][i].x, &oman.objects[AM][i].y, &oman.objects[AM][i].type, &oman.objects[AM][i].button.Type, &oman.objects[AM][i].button.Value) == EOF)
					return oman;

				if (oman.objects[AM][i].type == TButton || oman.objects[AM][i].type == TButtonWithFrame)
				{
					oman.objects[AM][i].button.Picture = load_picture(oman.objects[AM][i].button.Picture, c);
					oman.objects[AM][i].picture = oman.objects[AM][i].button.Picture;

					if (freadString(f, &c, "#", 50) < 0)
						return oman;
					
					oman.objects[AM][i].button.Clicked_Picture = load_picture(oman.objects[AM][i].button.Clicked_Picture, c);
				}
				else
					oman.objects[AM][i].picture = load_picture(oman.objects[AM][i].picture, c);

				fscanf(f, "\n");

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
				if (fscanf(f, "%d#%d#", &oman.objects[AM][i].x, &oman.objects[AM][i].y) == EOF)
					return oman;
				if (freadString(f, &oman.objects[AM][i].label.Text, "#", 50) < 0)
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

int paint_screen(SDL_Surface *_screen, struct Object objects[])
{
	_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	SDL_FillRect(_screen, NULL, _bg_color);

	int i = 0;
	while (!IS_NULL(objects[i]))
	{
		if (objects[i].enabled)
		{
			switch (objects[i].type)
			{
			case TButton: case TCard: // Button
				if (objects[i].button.Clicked)
					SDL_BlitSurface(objects[i].button.Clicked_Picture.picture, NULL, _screen, Create_Rect_BO(&objects[i], 0)); // Draw a clicked button
				else
					SDL_BlitSurface(objects[i].button.Picture.picture, NULL, _screen, Create_Rect_BO(&objects[i], 0)); // Foreground of the card
				break; 
			case TButtonWithFrame:
				// Objects should be able to get selected
				SDL_BlitSurface(objects[i].button.Picture.picture, NULL, _screen, Create_Rect_BO(&objects[i], 0)); // Draw a clicked button
				if (objects[i].button.Clicked)
					SDL_BlitSurface(objects[i].button.Clicked_Picture.picture, NULL, _screen, Create_Rect_BO(&objects[i], 1)); // Draw a clicked button
				break;
			default:
				SDL_BlitSurface(objects[i].picture.picture, NULL, _screen, Create_Rect_BO(&objects[i], 0)); // Draws everything else
				break;
			}
		}
		i++;
	}

	SDL_Flip(_screen);

	return 0;
}

int dist2object(struct Object objects[], int x, int y, int type[], int AmOfTypes)
{
	int i = 0;
	while (!IS_NULL(objects[i]))
	{
		for (int j = 0; j < AmOfTypes; j++)
		{
			if (objects[i].enabled && objects[i].type == type[j] || (!objects[i].button.Clicked && type[j] == TCard))
			{
				double xrel = x - objects[i].x;
				double yrel = y - objects[i].y;
				if (xrel > 0 && xrel < (objects[i].picture).picture->w)
				if (yrel > 0 && yrel < (objects[i].picture).picture->h)
					return i;
			}
		}
		i++;
	}
	return -1;
}

int Change_Menu(struct Objectmanager *oman, SDL_Surface *_screen, int Menu, int *Akt_Button)
{
	oman->objects[oman->Akt_Menu][*Akt_Button].button.Clicked = 0;
	*Akt_Button = -1;

	oman->Akt_Menu = Menu;

	paint_screen(_screen, oman->objects[oman->Akt_Menu]);
	return -1;
}

struct Objectmanager init_cards(struct Objectmanager *oman, struct Pair(*cards)[], char FileName[], struct Picture Background, int Menu)
{
	if (FileName[0] != '\0') // Dont laod cards out of txt-file
	{
		for (int i = 0; i < ARRAY_LENGTH; i++)
		{
			(*cards)[i].picture1 = (struct Picture *)malloc(sizeof(struct Picture));
			(*cards)[i].picture2 = (struct Picture *)malloc(sizeof(struct Picture));
			(*cards)[i].picture1->picture = NULL;
			(*cards)[i].picture2->picture = NULL;
		}
		ReadDeck(cards, FileName);
	}

	int index = 0;
	while (!IS_NULL(oman->objects[Menu][index++]));
	index--;

	struct Object objects[ARRAY_LENGTH];
	int j = 0;
	while ((*cards)[j].id != -1)
	{
		oman->objects[Menu][index] = O_New_Card(oman->objects[Menu][index], 0, 0, (*cards)[j], *(*cards)[j].picture1, Background);

		if (objects[index++].picture.picture == NULL)
			return *oman;

		oman->objects[Menu][index] = O_New_Card(oman->objects[Menu][index], 0, 0, (*cards)[j], *(*cards)[j].picture2, Background);

		if (objects[index++].picture.picture == NULL)
			return *oman;
		j++;
	}

	return *oman;
}
