#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "memory.h"
#include "Card.h"
#include "Object.h"
#include "Picture.h"

#pragma warning( disable : 4996 )
#pragma comment(lib, "winmm.lib") // For the sound

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   1024
#define SCREEN_BPP      32

SDL_Rect Gamefield;

/* function declarations */
int init_game(int AmPlayers, struct Card stack[], int AmCards, int AmX, int AmY);
int start_game(int amplayers, int card_background_id, struct Card stack[], int AmCards, int SizeX, int SizeY);
int dist2card(int x, int y, int type[], int AmOfTypes);
int paint_screen();
char* concat(char *s1, char *s2);
int Mouse_Clicked(int *mod, int *card1, int *card2, SDL_Event event, int amplayers);
int Mouse_Motion(int *mod, int *card1, int *card2, SDL_Event event, int amplayers);

struct Object objects[1000];

int pairs[8];
int textfield[8][2];

SDL_Rect PlayerPointsPosition[8];
SDL_Rect ButtonPosition[2];

struct Button *Buttons[4];

int AktPlayer;

int ClickedButton = -1;

struct Picture *Card_Background;
struct Picture Pic_Button;
struct Picture Pic_Button_Clicked;

int start_game(int amplayers, struct Card stack[], int AmCards, int SizeX, int SizeY, struct Picture *BG)
{
	Card_Background = BG;

	SDL_Event event;  /* event handling structure */
	int *mod = (int *)malloc(sizeof(int)); // 0 = first card, 1 = second card, 2 = turn cards back or remove them
	*mod = 0; 
	int *card1 = (int *)malloc(sizeof(int));
	*card1 = -1;
	int *card2 = (int*)malloc(sizeof(int));
	card2 = -1;

	if (amplayers > 8)
		return TOO_MANY_PLAYERS;

	int result = init_game(amplayers, stack, AmCards, SizeX, SizeY);
	if (result)
		return result;

	paint_screen();
	
	while (SDL_WaitEvent(&event))
	{
		if (event.type == SDL_QUIT)
			break;
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			int result = Mouse_Clicked(mod, card1, card2, event, amplayers);
			if (result == -1)
				break;

		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			Mouse_Motion(mod, card1, card2, event, amplayers);
		}
	}
}

int Mouse_Motion(int *mod, int *card1, int *card2, SDL_Event event, int amplayers)
{
	// Check for the button
	int Types[] = { 2 };
	int actbutton = dist2card(event.button.x, event.button.y, Types, 1);
	if (actbutton > -1)
	{
		if (actbutton != ClickedButton)
		{
			objects[actbutton].button.Clicked = 1;
			ClickedButton = actbutton;
		}
	}
	else if (ClickedButton > -1) // Reset clicked button, after mouse left it
	{
		objects[ClickedButton].button.Clicked = 0;
		ClickedButton = -1;
	}
	paint_screen();
}

int Mouse_Clicked(int *mod, int *card1, int *card2, SDL_Event event, int amplayers)
{
	int Types[] = { 1, 2 };
	int actcard = dist2card(event.button.x, event.button.y, Types, 2);

	if (objects[actcard].type == 2) // Button
	{
		switch (objects[actcard].button.Type) // Beenden
		{
		case 0: return -1;
		case 1: break;
		}
	}
	else
	{
		switch (*mod)
		{
		case 0:	case 1: // tap card
			if (actcard > -1 && objects[actcard].enabled && !objects[actcard].card.visible)
			{
				if (mod == 0)
					*card1 = actcard;
				else
					*card2 = actcard;

				objects[actcard].card.visible = 1;
				mod++;
			}
			break;

		case 2: // Remove them or move them back
			if (objects[*card1].card.type == objects[*card2].card.type)
			{
				objects[*card1].enabled = 0;
				objects[*card2].enabled = 0;

				pairs[AktPlayer]++;

				char *c = (char *)malloc(sizeof(char));
				sprintf(c, "%d", pairs[AktPlayer]);
				Create_Picture_By_Text(objects[textfield[AktPlayer][1]].picture, concat(c, " pairs found"), 0);
			}
			else
			{
				objects[*card1].card.visible = 0;
				objects[*card2].card.visible = 0;

				char *c = (char *)malloc(sizeof(char));
				sprintf(c, "%d", AktPlayer + 1);
				Create_Picture_By_Text(objects[textfield[AktPlayer][0]].picture, concat("Player ", c), 0);

				AktPlayer++;
				if (AktPlayer == amplayers) AktPlayer = 0;

				sprintf(c, "%d", AktPlayer + 1);
				Create_Picture_By_Text(objects[textfield[AktPlayer][0]].picture, concat("Player ", c), 1);
			}
			mod = 0;
			break;
		default: break;
		}
	}
	paint_screen();
}

int dist2card(int x, int y, int type[], int AmOfTypes)
{
	int i = 0;
	while (!IS_NULL(objects[i]))
	{
		for (int j = 0; j < AmOfTypes; j++)
		{
			if (objects[i].enabled && !objects[i].card.visible && objects[i].type == type[j])
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

char* concat(char *s1, char *s2)
{
	char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);//+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);

	return result;
}

int init_game(int AmPlayers, struct Card stack[], int AmCards, int AmX, int AmY)
{
	FILE *f;
	f = fopen("./resources/config.txt", "r");

	int i = 0;
	// Loading Background
	char c[100];
	if (fscanf(f, "%s %3d %3d %3d", &c, &objects[i].x, &objects[i].y, &objects[i].type) != EOF)
	{
		objects[i].picture = load_picture(objects[i].picture, c);
		if (objects[i].picture.picture == NULL)
			return FAILED_LOADING_BACKGROUND;

		objects[i].enabled = 1;
		i++;
	}

	if (fscanf(f, "%d %d %d %d", &Gamefield.x, &Gamefield.y, &Gamefield.w, &Gamefield.h) == EOF)
		return FAILED_LOADING_GAMEFIELD;

	for (int j = 0; j < 8; j++)
	if (fscanf(f, "%d %d", &PlayerPointsPosition[j].x, &PlayerPointsPosition[j].y) == EOF)
		return FAILED_LOADING_PlAYERPOINTSPOSITION;
	
	srand(time(NULL));  /* start random number generater */

	AktPlayer = rand() % (AmPlayers + 1);

	// Paint the player-labels
	for (int j = 0; j < AmPlayers; j++)
	{
		objects[i].x = PlayerPointsPosition[j].x;
		objects[i].y = PlayerPointsPosition[j].y;
		objects[i].type = 100;
		objects[i].enabled = 1;
		char *c = (char *)malloc(sizeof(char));
		sprintf(c, "%d", j + 1);
		objects[i].picture = Create_Picture_By_Text(objects[i].picture, concat("Player ", c), j == AktPlayer ? 1 : 0);

		pairs[j] = 0;
		textfield[j][0] = i++;

		objects[i].x = PlayerPointsPosition[j].x;
		objects[i].y = 30 + PlayerPointsPosition[j].y;
		objects[i].type = 101;
		objects[i].enabled = 1;
		objects[i].picture = Create_Picture_By_Text(objects[i].picture, "0 pairs found", 0);

		pairs[j] = 0;
		textfield[j][1] = i++;
	}

	if (fscanf(f, "%s", &c) == EOF)
		return FAILED_LOADING_PIC_BUTTON_ADDRESS;

	Pic_Button = load_picture(Pic_Button, c);

	if (fscanf(f, "%s", &c) == EOF)
		return FAILED_LOADING_PIC_BUTTON_CLICKED_ADDRESS;

	Pic_Button_Clicked = load_picture(Pic_Button_Clicked, c);

	// Load the buttons
	for (int j = 0; j < 4; j++)
	{
		if (fscanf(f, "%d %d %d %d %s %d", &ButtonPosition[j].x, &ButtonPosition[j].y, &ButtonPosition[j].w, &ButtonPosition[j].h, &c, &objects[i].button.Type) == EOF)
			return -1;

		objects[i].x = ButtonPosition[j].x;
		objects[i].y = ButtonPosition[j].y;
		objects[i].type = 2;
		objects[i].enabled = 1;
		objects[i].picture = Pic_Button;
		objects[i].button.Picture = &Pic_Button;
		objects[i].button.Clicked_Picture = &Pic_Button_Clicked;
		objects[i].button.Text_Picture = Create_Picture_By_Text(objects[i].button.Text_Picture, c, 0);
		objects[i].button.Clicked = 0;
		i++;
	}


	// Load Objects

	while (fscanf(f, "%s %3d %3d %3d", &c, &objects[i].x, &objects[i].y, &objects[i].type) != EOF)
	{
		objects[i].picture = load_picture(objects[i].picture, c);
		if (objects[i].picture.picture == NULL)
			return FAILED_LOADING_IMAGE;

		objects[i].enabled = 1;
		i++;
	}

	// Shuffle Cards in Stack - Knuth-Fisher-Yates shuffle **/
	for (int j = AmCards - 1; j; j--) {
		int k = rand() % (j + 1);  /* random variable modulo remaining cards */
		/* swap entries of fields i and j */
		struct Card swap = stack[j];
		stack[j] = stack[k];
		stack[k] = swap;
	}

	// Draw Cards on gamefield

	int j = 0;
	int SizeX = (*stack[0].picture).picture->w;
	int SizeY = (*stack[0].picture).picture->h;

	for (int x = 0; x < AmX; x++)
	for (int y = 0; y < AmY; y++)
	{
		if ((*stack[j].picture).picture == NULL)
			break;

		objects[i].type = 1; // objects is card
		objects[i].card = stack[j++];
		objects[i].picture = *objects[i].card.picture;
		objects[i].card.picture = &objects[i].picture;
		objects[i].enabled = 1;
		objects[i].x = Gamefield.x + Gamefield.w / AmX * x;
		objects[i].y = Gamefield.y + Gamefield.h / AmY * y;
		i++;
	}

	return 0;
}


int paint_screen()
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
			case 1: // card
				if (objects[i].card.visible)
					SDL_BlitSurface(objects[i].picture.picture, NULL, _screen, Create_Rect_BO(objects[i], 0, 0)); // Foreground of the card
				else
					SDL_BlitSurface((*Card_Background).picture, NULL, _screen, Create_Rect_BO(objects[i], 0, 0)); // Background of the card
				break;
			case 2: // Button
				if (objects[i].button.Clicked)
					SDL_BlitSurface(objects[i].button.Clicked_Picture->picture, NULL, _screen, Create_Rect_BO(objects[i], 0, 0)); // Draw a clicked button
				else
					SDL_BlitSurface(objects[i].button.Picture->picture, NULL, _screen, Create_Rect_BO(objects[i], 0, 0)); // Foreground of the card

				SDL_BlitSurface(objects[i].button.Text_Picture.picture, NULL, _screen, Create_Rect_BO(objects[i], 5, 5)); // Foreground of the card
				break;
			default:
				SDL_BlitSurface(objects[i].picture.picture, NULL, _screen, Create_Rect_BO(objects[i], 0, 0)); // Draws everything else
				break;
			}
		}
		i++;
	}

	SDL_Flip(_screen);

	return 0;
}