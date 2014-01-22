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
int init_game(int AmPlayers, struct Card (*stack)[], int AmCards, int AmX, int AmY);
int start_game(int amplayers, int card_background_id, struct Card (*stack)[], int AmCards, int SizeX, int SizeY, int Loading_Game);
int paint_screen();
char* concat(char *s1, char *s2);
int Mouse_Clicked(int *mod, int *card1, int *card2, struct Card *cards[], int amCards, SDL_Event event, int amplayers);
int Mouse_Motion(int *mod, int *card1, int *card2, SDL_Event event, int amplayers);
int Save_Game(int amPlayers, struct Card *cards[]);
int Load_Game();
struct Object objects[ARRAY_LENGTH];

int pairs[8];
int NuOfRePairs;
int PosOfReCards;

int textfield[8][2]; // ID for the Textfields of: PlayerID, 2nd: 0-PlayerNumber, 1-Pairstext

SDL_Rect PlayerPointsPosition[8];

struct Button *Buttons[4];

int AktPlayer;

int ClickedButton = -1;

struct Picture Pic_Button;
struct Picture Pic_Button_Clicked;

int start_game(int amplayers, struct Card (*stack)[], int AmCards, int SizeX, int SizeY, struct Picture *BG, int Loading_Game)
{
	// SizeX && SizeY are the amount of Cards in x and y direction on the gamefield
	Card_Background = *BG;

	SDL_Event event;  /* event handling structure */
	int *mod = (int *)malloc(sizeof(int)); // 0 = first card, 1 = second card, 2 = turn cards back or remove them
	*mod = 0; 
	int *card1;
	card1 = (int *)malloc(sizeof(int));
	*card1 = -1;
	int *card2;
	card2 = (int*)malloc(sizeof(int));
	*card2 = -1;
	
	if (amplayers > 8)
		return TOO_MANY_PLAYERS;

	int result = init_game(amplayers, stack, AmCards, SizeX, SizeY);
	if (result)
		return result;

	paint_screen(_screen, &objects);
	
	while (SDL_WaitEvent(&event))
	{
		if (event.type == SDL_QUIT)
			break;
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			int result = Mouse_Clicked(mod, card1, card2, &stack, AmCards, event, amplayers, SizeX, SizeY);
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
	int actbutton = dist2object(&objects, event.button.x, event.button.y, Types, 1);
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
	paint_screen(_screen, &objects);
}

int Save_Game(int amPlayers, int amCards, struct Card *cards[], int GameSizeX, int GameSizeY)
{
	FILE *f = fopen("Savegame.txt", "w");
	if (fprintf(f, "") == EOF)
		return -1;

	fprintf(f, "%d %d %d %s", amPlayers, GameSizeX, GameSizeY, Card_Background.filename);

	Save_Objects(objects, f);
	
	//objects
	//Gamefield
	//PlayerPointsPosition
	//AktPlayer
	//textfield
	//Pic_Button
	//Pic_Button_Clicked
	//pairs
	//textfield
}

int Load_Game()
{
	FILE *f = fopen("Savegame.txt", "r");
	if (fprintf(f, "") == EOF)
		return -1;
	
	int GameSizeX = 0, GameSizeY = 0, amPlayers = 0;
	char FileName[FILENAME_LENGHT];
	if (fscanf(f, "%d %d %d %s", amPlayers, GameSizeX, GameSizeY, &FileName) == EOF)
		return FAILED_LOADING_SAVEGAME;

	struct Card cards[ARRAY_LENGTH];
	int amCards = init_cards(&cards, "Savegame_Cards.txt");
	if (amCards < 0)
		return amCards;

	Card_Background = load_picture(Card_Background, FileName);

	start_game(amPlayers, cards, amCards, GameSizeX, GameSizeY, &Card_Background, 1);

	return 0;
}

int Mouse_Clicked(int *mod, int *card1, int *card2, struct Card *cards[], int amCards, SDL_Event event, int amplayers, int GameSizeX, int GameSizeY)
{
	int Types[] = { 1, 2 };
	int actcard = dist2object(&objects, event.button.x, event.button.y, Types, 2);

	if (objects[actcard].type == 2) // Button
	{
		switch (objects[actcard].button.Type) // Beenden
		{
		case 0: return -1;
		case 1: Save_Game(amplayers, amCards, cards, GameSizeX, GameSizeY); break;
		case 2: Load_Game(); break;
		default:	break;
		}
	}
	else
	{
		switch (*mod)
		{
		case 0:	case 1: // tap card
			if (actcard > -1 && objects[actcard].enabled && !objects[actcard].card.visible)
			{
				if (*mod == 0)
					*card1 = actcard;
				else
					*card2 = actcard;

				objects[actcard].card.visible = 1;
				(*mod)++;
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
				objects[textfield[AktPlayer][1]].picture = Create_Picture_By_Text(objects[textfield[AktPlayer][1]].picture, concat(c, " pairs found"), 0);

				NuOfRePairs--;
				*c = (char *)malloc(sizeof(char));
				sprintf(c, "%d", NuOfRePairs);
				objects[PosOfReCards].picture = Create_Picture_By_Text(objects[PosOfReCards].picture, concat("Amount of remaining pairs: ", c), 0);

				if (NuOfRePairs == 0) // Game ends
				{
					GameEnd(amplayers);
				}
			}
			else
			{
				objects[*card1].card.visible = 0;
				objects[*card2].card.visible = 0;

				char *c = (char *)malloc(sizeof(char));
				sprintf(c, "%d", AktPlayer + 1);
				objects[textfield[AktPlayer][0]].picture = Create_Picture_By_Text(objects[textfield[AktPlayer][0]].picture, concat("Player ", c), 0);

				AktPlayer++;
				if (AktPlayer == amplayers) AktPlayer = 0;

				sprintf(c, "%d", AktPlayer + 1);
				objects[textfield[AktPlayer][0]].picture = Create_Picture_By_Text(objects[textfield[AktPlayer][0]].picture, concat("Player ", c), 1);
			}
			*mod = 0;
			break;
		default: break;
		}
		paint_screen(_screen, &objects);
	}
}

int GameEnd(int amPlayers)
{
	int iwinner;
	int iPairs = 0;
	for (int i = 0; i < amPlayers; i++)
	if (pairs[i] > iPairs)
	{
		iwinner = i;
		iPairs = pairs[iwinner];
	}

	int i = 0;
	while (!IS_NULL(objects[i++]));
	i--;

	char c[100];

	char *c2 = (char *)malloc(sizeof(char));
	sprintf(c2, "%d", iPairs);
	concat("The Winner with ", c2);
	if (iPairs == 1)
		concat(c, " pair is Player ");
	else
		concat(c, " pairs is Player ");
	
	sprintf(c2, "%d", iwinner);
	concat(c, c2);

	objects[i] = O_New_Label(objects[i], c, 300, 300);

	i++;
	objects[i] = O_New_Button(objects[i], "Continue", BContinue, 300, 400);
}

char* concat(char *s1, char *s2)
{
	char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);//+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);

	return result;
}

int init_game(int AmPlayers, struct Card (*stack)[], int AmCards, int AmX, int AmY)
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

	int X, Y;
	if (fscanf(f, "%d %d", &X, &Y) != EOF)
	{
		objects[i].x = X;
		objects[i].y = Y;
		objects[i].type = 0;
		objects[i].enabled = 1;
		char *c = (char *)malloc(sizeof(char));

		NuOfRePairs = AmCards / 2; 
		sprintf(c, "%d", NuOfRePairs);
		objects[i].picture = Create_Picture_By_Text(objects[i].picture, concat("Number of remaining pairs: ", c), 0);

		PosOfReCards = i++;
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
		objects[i].type = 2;
		objects[i].enabled = 1;

		int x = 0, y = 0;
		if (fscanf(f, "%d %d %s %d", &x, &y, &c, &objects[i].button.Type) == EOF)
			return -1;

		objects[i].button = New_Button(objects[i].button, &Pic_Button, &Pic_Button_Clicked);
		objects[i].picture = objects[i].button.Picture;
		objects[i].x = x;
		objects[i].y = y;

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
		struct Card swap = (*stack)[j];
		(*stack)[j] = (*stack)[k];
		(*stack)[k] = swap;
	}

	// Draw Cards on gamefield

	int j = 0;
	int SizeX = (*(*stack)[0].picture).picture->w;
	int SizeY = (*(*stack)[0].picture).picture->h;

	for (int x = 0; x < AmX; x++)
	for (int y = 0; y < AmY; y++)
	{
		if ((*(*stack)[j].picture).picture == NULL)
			break;

		objects[i].type = 1; // objects is card
		objects[i].card = (*stack)[j++];
		objects[i].picture = *objects[i].card.picture;
		objects[i].card.picture = &objects[i].picture;
		objects[i].enabled = 1;
		objects[i].x = Gamefield.x + Gamefield.w / AmX * x;
		objects[i].y = Gamefield.y + Gamefield.h / AmY * y;
		i++;
	}

	return 0;
}