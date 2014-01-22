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

#define Number_Of_Menues 2

struct Object objects[Number_Of_Menues][ARRAY_LENGTH];


#define MAIN_MENU 0
#define HIGH_SCORE 1

int Akt_Menu = 0;


int init_card_background();
int init_menu();
int Paint_Highscore();

int Buttons[Number_Of_Menues][ARRAY_LENGTH];
int NumberOfButtons[Number_Of_Menues];

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

	//PlaySound(L"./resources/music/Bioweapon - Heretic.wav", NULL, SND_LOOP | SND_ASYNC);

	init_card_background();
	init_cards(&cards[0], "./resources/cards/cards.txt");
	init_menu();
	
	SDL_Event event;

	int Akt_Button = -1;

	while (SDL_WaitEvent(&event))
	{
		if (event.type == SDL_QUIT)
			break;
		
		char* c = (char*)malloc(10*sizeof(char));
		sprintf(c, "%d", event.key.keysym.sym);
		SDL_WM_SetCaption(c, NULL);

		if ((event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN))
		{
			int actobject;
			if (event.key.keysym.sym == SDLK_RETURN && Akt_Button > -1)
			{
				actobject = Buttons[Akt_Menu][Akt_Button];
			}
			else
			{
				int Types[] = { 2 };
				actobject = dist2object(&objects[Akt_Menu], event.button.x, event.button.y, Types, 1);
			}

			if (actobject > -1)
			{
				if (objects[Akt_Menu][actobject].button.Type == BStart) // Start game
				{
					int result = start_game(2, cards, 4, 7, 7, &card_background[0], 0);
					if (result)
						return result;
				}
				else if (objects[Akt_Menu][actobject].button.Type == BHighscore)
				{
					Akt_Menu = HIGH_SCORE;
					Paint_Highscore();
					paint_screen(_screen, &objects[Akt_Menu]);
				}
				else if (objects[Akt_Menu][actobject].button.Type == BMainMenu)
				{
					Akt_Menu = MAIN_MENU;
					paint_screen(_screen, &objects[Akt_Menu]);
				}
				else if (objects[Akt_Menu][actobject].button.Type == BExit)
				{
					return 0;
				}
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			// Check for the button
			int Types[] = { 2 };
			int newbutton = dist2object(&objects, event.button.x, event.button.y, Types, 1);
			if (newbutton > -1)
			{
				if (newbutton != Akt_Button)
				{
					objects[Akt_Menu][newbutton].button.Clicked = 1;
					Akt_Button = newbutton;
				}
			}
			else if (Akt_Button > -1) // Reset clicked button, after mouse left it
			{
				objects[Akt_Menu][Akt_Button].button.Clicked = 0;
				Akt_Button = -1;
			}
			paint_screen(_screen, &objects);			
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				if (Akt_Button > -1)
					objects[Akt_Menu][Buttons[Akt_Menu][Akt_Button]].button.Clicked = 0;

				Akt_Button++;
				if (Akt_Button >= NumberOfButtons[Akt_Menu])
					Akt_Button = 0;

				objects[Akt_Menu][Buttons[Akt_Menu][Akt_Button]].button.Clicked = 1;

				paint_screen(_screen, &objects[Akt_Menu]);
			}
			else if (event.key.keysym.sym == SDLK_UP)
			{
				if (Akt_Button > -1)
					objects[Akt_Menu][Buttons[Akt_Menu][Akt_Button]].button.Clicked = 0;

				Akt_Button--;
				if (Akt_Button < 0)
					Akt_Button = NumberOfButtons[Akt_Menu] - 1;

				objects[Akt_Menu][Buttons[Akt_Menu][Akt_Button]].button.Clicked = 1;

				paint_screen(_screen, &objects);
			}
		}
	}


	SDL_Quit();

	return 0;
}

int Paint_Highscore()
{
	int i = 0;
	while (!IS_NULL(objects[Akt_Menu][i])) 
	{
		if (objects[Akt_Menu][i].type == THighscoreitem)
			objects[Akt_Menu][i].enabled = 0;
		i++;
	}
	struct Object *o = GetHighscoreItems();
	for (int j = 0; j < HS_LENGTH * 2; j++)
		objects[Akt_Menu][i++] = *(o++);

	return -1;
}

int init_menu()
{
	// Reset objects
	for (int j = 0; j < Number_Of_Menues; j++)
	for (int i = 0; i < ARRAY_LENGTH; i++)
		objects[j][i].picture.picture = NULL;

	FILE *f;
	f = fopen("./resources/menu_config.txt", "r");	
	// Loading Background
	char c[100];

	// Load the different Menues

	int AM = 0;
	while (fscanf(f, "%d", &AM) != EOF)
	{
		int i = 0;
		// Load the buttons
		fscanf(f, "%s", &c);
		if (c[0] == 'B')
		{
			fscanf(f, "%d", &NumberOfButtons[AM]);

			for (int j = 0; j < NumberOfButtons[AM]; j++)
			{
				int x = 0, y = 0, Type = 0;
				if (fscanf(f, "%d %d %s %d", &x, &y, &c, &Type) == EOF)
					return -1;

				objects[AM][i] = O_New_Button(objects[AM][i], c, Type, x, y);
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
				if (fscanf(f, "%s %d %d %d", &c, &objects[AM][i].x, &objects[AM][i].y, &objects[AM][i].type) == EOF)
					return -1;

				objects[AM][i].picture = load_picture(objects[AM][i].picture, c);
				if (objects[AM][i].picture.picture == NULL)
					return FAILED_LOADING_BACKGROUND;

				objects[AM][i].enabled = 1;
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
				if (fscanf(f, "%d %d %s", &objects[AM][i].x, &objects[AM][i].y, &objects[AM][i].label.Text) == EOF)
					return -1;

				objects[AM][i].picture = Create_Picture_By_Text(objects[AM][i].picture, objects[AM][i].label.Text, 0);
				if (objects[AM][i].picture.picture == NULL)
					return FAILED_LOADING_BACKGROUND;

				objects[AM][i].enabled = 1;
				i++;
			}
		}
	}

	paint_screen(_screen, &objects[Akt_Menu]);
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
