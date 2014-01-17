#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "memory.h"
#include "Card.h"
#include "Picture.h"
#include "Object.h"

#pragma warning( disable : 4996 )
#pragma comment(lib, "winmm.lib") // For the sound

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

struct Picture card_background[ARRAY_LENGTH];
struct Card cards[ARRAY_LENGTH];
struct Object objects[ARRAY_LENGTH];

int init_card_background();
int init_menu();



struct Picture *Card_Background;
struct Picture Pic_Button;
struct Picture Pic_Button_Clicked;

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL konnte nicht initialisiert werden:  %s\n", SDL_GetError());
		return INITIALIZATION_FAILURE;
	}

	_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (_screen == NULL)
	{
		fprintf(stderr, "SDL konnte nicht initialisiert werden:  %s\n", SDL_GetError());
		return NO_SCREEN;
	}

	//Initialize SDL_ttf 
	if (TTF_Init() == -1)
	{
		fprintf(stderr, "SDL_TTF konnte nicht initialisiert werden:  %s\n", TTF_GetError());
		return INITIALIZATION_FAILURE;
	}
	//Set the window caption 

	//SDL_WM_SetCaption("Mathe Memory", NULL); 

	// Loads the Font
	font = TTF_OpenFont("./resources/lazy.ttf", 28);
	if (font == NULL)
	{
		fprintf(stderr, "SDL_TTF konnte nicht initialisiert werden:  %s\n", TTF_GetError());
		return FONT_LOAD_FAILURE;
	}

	_bg_color = SDL_MapRGB(_screen->format, 0, 0, 0); // Sets the backgroundcolor to black

	PlaySound(L"./resources/music/Bioweapon - Heretic.wav", NULL, SND_LOOP | SND_ASYNC);

	init_card_background();
	init_cards(&cards[0], "./resources/cards/cards.txt");
	init_menu();
	
	SDL_Event event;

	while (SDL_WaitEvent(&event))
	{
		if (event.type == SDL_QUIT)
			break;
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			int Types[] = { 2 };
			int actobject = dist2object(&objects, event.button.x, event.button.y, Types, 1);
			if (actobject > -1)
			{
				if (objects[actobject].button.Type == 0) // Start game
				{
					int result = start_game(8, cards, 100, 10, 10, &card_background[0], 0);
					if (result)
						return result;
				}
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			
		}
	}


	SDL_Quit();

	return 0;
}


int init_menu(){
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		objects[i].picture.picture = NULL;
	}
	FILE *f;
	f = fopen("./resources/menu_config.txt", "r");
	int i = 0;
	// Loading Background
	char c[100];
	if (fscanf(f, "%s", &c) != EOF)
	{
		Pic_Button = load_picture(objects[i].picture, c);
		if (Pic_Button.picture == NULL)
			return FAILED_LOADING_BACKGROUND;
	}

	if (fscanf(f, "%s", &c) != EOF)
	{
		Pic_Button_Clicked = load_picture(objects[i].picture, c);
		if (Pic_Button_Clicked.picture == NULL)
			return FAILED_LOADING_BACKGROUND;
	}

	// Load the buttons
	for (int j = 0; j < 4; j++)
	{
		objects[i].type = 2;
		objects[i].enabled = 1;
		objects[i].picture = Pic_Button;

		int x = 0, y = 0, w = 0, h = 0;
		if (fscanf(f, "%d %d %d %d %s %d", &x, &y, &w, &h, &c, &objects[i].button.Type) == EOF)
			return -1;

		objects[i].button.Picture = (struct Picture *)malloc(sizeof(struct Picture));
		objects[i].button.Clicked_Picture = (struct Picture *)malloc(sizeof(struct Picture));
		objects[i].button.Text_Picture.picture = (struct Picture *)malloc(sizeof(struct Picture));
			objects[i].button = New_Button(objects[i].button, &Pic_Button, &Pic_Button_Clicked, Create_Picture_By_Text(objects[i].button.Text_Picture, c, 0));

		objects[i].x = x;
		objects[i].y = y;
		objects[i].button.x = w;
		objects[i].button.y = h;

		i++;
	}

	while (fscanf(f, "%s %3d %3d %3d", &c, &objects[i].x, &objects[i].y, &objects[i].type) != EOF)
	{
		objects[i].picture = load_picture(objects[i].picture, c);
		if (objects[i].picture.picture == NULL)
			return FAILED_LOADING_BACKGROUND;

		objects[i].enabled = 1;
		i++;
	}

	paint_screen(_screen, &objects);
}

int init_card_background()
{
	FILE *f;
	f = fopen("./resources/card_backgrounds/card_backgrounds.txt", "r");

	for (int i = 0; i < ARRAY_LENGTH; i++)
		card_background[i].picture = NULL;

	int i = 0;
	char c[102];
	while (fscanf(f, "%s", &c) != EOF)
	{
		card_background[i] = load_picture(card_background[i], c);

		if (card_background[i].picture == NULL)
			return FAILED_LOADING_IMAGE;

		i++;
	}

	return 0;
}
