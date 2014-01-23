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
#include "Objectmanager.h"
#include "Highscore.h"

#pragma warning( disable : 4996 )
#pragma comment(lib, "winmm.lib") // For the sound

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

struct Picture card_background[ARRAY_LENGTH];
struct Card cards[ARRAY_LENGTH];

struct Objectmanager oman;

int init_card_background();
int init_menu();
int Paint_Highscore();


int SoundOn = 1;

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

	SDL_WM_SetCaption("Mathe Memory", NULL); 

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

		if ((event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN))
		{
			int actobject;
			if (event.key.keysym.sym == SDLK_RETURN && oman.Akt_Button > -1)
			{
				actobject = oman.Akt_Button;
			}
			else
			{
				int Types[] = { 2 };
				actobject = dist2object(event.button.x, event.button.y, Types, 1);
			}

			if (actobject > -1)
			{
				if (oman.objects[oman.Akt_Menu][actobject].button.Type == BStart) // Start game
				{
					int result = start_game(2, cards, 4, 7, 7, &card_background[0], 0);
					if (result)
						return result;
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BHighscore)
				{
					Paint_Highscore();
					Chane_Menu(HIGH_SCORE, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BMainMenu)
				{
					Chane_Menu(MAIN_MENU, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BConfigurations)
				{
					Chane_Menu(CONFIGURATIONS, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSoundOn)
				{
					PlaySound(L"./resources/music/Bioweapon - Heretic.wav", NULL, SND_LOOP | SND_ASYNC);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSoundOff)
				{
					PlaySound(NULL, NULL, SND_LOOP | SND_ASYNC);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BExit)
				{
					return 0;
				}
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			// Check for the button
			int Types[] = { 2 };
			int newbutton = dist2object(oman.Akt_Menu, event.button.x, event.button.y, Types, 1);
			if (newbutton > -1)
			{
				if (newbutton != oman.Akt_Button)
				{
					oman.objects[oman.Akt_Menu][newbutton].button.Clicked = 1;
					oman.Akt_Button = newbutton;
				}
			}
			else if (oman.Akt_Button > -1) // Reset clicked button, after mouse left it
			{
				oman.objects[oman.Akt_Menu][oman.Akt_Button].button.Clicked = 0;
				oman.Akt_Button = -1;
			}
			paint_screen(_screen, oman.Akt_Menu);
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				if (oman.Akt_Button > -1)
					oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 0;

				oman.Akt_Button++;
				if (oman.Akt_Button >= oman.NumberOfButtons[oman.Akt_Menu])
					oman.Akt_Button = 0;

				oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 1;

				paint_screen(_screen, oman.Akt_Menu);
			}
			else if (event.key.keysym.sym == SDLK_UP)
			{
				if (oman.Akt_Button > -1)
					oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 0;

				oman.Akt_Button--;
				if (oman.Akt_Button < 0)
					oman.Akt_Button = oman.NumberOfButtons[oman.Akt_Menu] - 1;

				oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 1;

				paint_screen(_screen, oman.Akt_Menu);
			}
		}
	}


	SDL_Quit();

	return 0;
}

int Chane_Menu(int Menu, int *Akt_Button)
{
	oman.objects[oman.Akt_Menu][*Akt_Button].button.Clicked = 0;
	*Akt_Button = -1;

	oman.Akt_Menu = Menu;
	
	paint_screen(_screen, oman.Akt_Menu);
	return -1;
}

int Paint_Highscore()
{
	int i = 0;
	while (!IS_NULL(oman.objects[oman.Akt_Menu][i]))
	{
		if (oman.objects[HIGH_SCORE][i].type == THighscoreitem)
			oman.objects[HIGH_SCORE][i].enabled = 0;
		i++;
	}
	struct Object *o = GetHighscoreItems();
	for (int j = 0; j < HS_LENGTH * 2; j++)
		oman.objects[HIGH_SCORE][i++] = *(o++);

	return -1;
}

int init_menu()
{
	// Reset objects
	for (int j = 0; j < Number_Of_Menues; j++)
	for (int i = 0; i < ARRAY_LENGTH; i++)
		oman.objects[j][i].picture.picture = NULL;

	FILE *f;
	f = fopen("./resources/menu_config.txt", "r");	
	
	// Load the different Menues
	oman = Load_Objects(oman, f);

	paint_screen(_screen, oman.Akt_Menu);
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
