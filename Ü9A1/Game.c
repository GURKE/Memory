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
#include "Objectmanager.h"
#include "Player.h"

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
int init_game(int AmPlayers, struct Card (*stack)[], int AmCards);
int start_game(int amplayers, struct Card(*stack)[], int AmCards, int SizeX, int SizeY, struct Picture *BG, int Loading_Game, struct Player _players[]);
int paint_screen();
char* concat(char *s1, char *s2);
int Mouse_Clicked(int *mod, int *card1, int *card2, struct Card *cards[], int amCards, SDL_Event event, int amplayers);
int Mouse_Motion(int *mod, int *card1, int *card2, SDL_Event event, int amplayers);
int Save_Game(int amPlayers, struct Card *cards[]);
int Load_Game();
int UpdateRemainingCards(int AmCards);

//struct Object objects[ARRAY_LENGTH];

int NuOfRePairs;  // amount of the remaining cards
int PosOfReCards; // ID of the object for the label of the reamaining cards

struct Button *Buttons[4];

int AktPlayer;

int ClickedButton = -1;


struct Picture Pic_Button;
struct Picture Pic_Button_Clicked;

struct Objectmanager oman2;
struct Player Players[8];

int start_game(int amplayers, struct Card (*stack)[], int AmCards, struct Picture *BG, int Loading_Game, struct Player _players[])
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
	
	for (size_t i = 0; i < amplayers; i++)
		Players[i] = _players[i];

	int result = init_game(amplayers, stack, AmCards);
	if (result)
		return result;

	paint_screen(_screen, oman2.objects[oman2.Akt_Menu]);
	
	while (SDL_WaitEvent(&event))
	{
		if (event.type == SDL_QUIT)
			break;
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			int result = Mouse_Clicked(mod, card1, card2, &stack, AmCards, event, amplayers);
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
	int actbutton = dist2object(oman2.objects[oman2.Akt_Menu], event.button.x, event.button.y, Types, 1);
	if (actbutton > -1)
	{
		if (actbutton != ClickedButton)
		{
			oman2.objects[oman2.Akt_Menu][actbutton].button.Clicked = 1;
			ClickedButton = actbutton;
		}
	}
	else if (ClickedButton > -1) // Reset clicked button, after mouse left it
	{
		oman2.objects[oman2.Akt_Menu][ClickedButton].button.Clicked = 0;
		ClickedButton = -1;
	}
	paint_screen(_screen, oman2.objects[oman2.Akt_Menu]);
}

int Save_Game(int amPlayers, int amCards, struct Card *cards[])
{
	FILE *f = fopen("Savegame.txt", "w");
	if (fprintf(f, "") == EOF)
		return -1;

	fprintf(f, "%d %s", amPlayers, Card_Background.filename);

	Save_Objects(oman2.objects[oman2.Akt_Menu], f);
	
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


	
	start_game(amPlayers, cards, amCards, GameSizeX, GameSizeY, &Card_Background, 1, Players);

	return 0;
}

int Mouse_Clicked(int *mod, int *card1, int *card2, struct Card *cards[], int amCards, SDL_Event event, int amplayers)
{
	int Types[] = { 1, 2 };
	int actcard = dist2object(oman2.objects[oman2.Akt_Menu], event.button.x, event.button.y, Types, 2);

	if (oman2.objects[oman2.Akt_Menu][actcard].type == 2) // Button
	{
		switch (oman2.objects[oman2.Akt_Menu][actcard].button.Type) // Beenden
		{
		case 12: Change_Menu(&oman2, _screen, EXIT, &oman2.Akt_Button); break;
		case 1: Save_Game(amplayers, amCards, cards); break;
		case 2: Load_Game(); break;
		default:	break;
		}
	}
	else
	{
		switch (*mod)
		{
		case 0:	case 1: // tap card
			if (actcard > -1 && oman2.objects[oman2.Akt_Menu][actcard].enabled && !oman2.objects[oman2.Akt_Menu][actcard].card.visible)
			{
				if (*mod == 0)
					*card1 = actcard;
				else
					*card2 = actcard;

				oman2.objects[oman2.Akt_Menu][actcard].card.visible = 1;
				(*mod)++;
			}
			break;

		case 2: // Remove them or move them back
			if (oman2.objects[oman2.Akt_Menu][*card1].card.type == oman2.objects[oman2.Akt_Menu][*card2].card.type)
			{ // Pair found
				oman2.objects[oman2.Akt_Menu][*card1].enabled = 0;
				oman2.objects[oman2.Akt_Menu][*card2].enabled = 0;

				Players[AktPlayer].FoundPairs++;

				char *c = (char *)malloc(sizeof(char));
				sprintf(c, "%d", Players[AktPlayer].FoundPairs);
				Players[AktPlayer].ObPlayerPoints->picture = Create_Picture_By_Text(Players[AktPlayer].ObPlayerPoints->picture, concat(c, " pairs found"), 0);

				NuOfRePairs--;
				*c = (char *)malloc(sizeof(char));
				sprintf(c, "%d", NuOfRePairs);
				oman2.objects[oman2.Akt_Menu][PosOfReCards].picture = Create_Picture_By_Text(oman2.objects[oman2.Akt_Menu][PosOfReCards].picture, concat("Amount of remaining pairs: ", c), 0);

				if (NuOfRePairs == 0) // Game ends
				{
					GameEnd(amplayers);
				}
			}
			else
			{ // No pair found
				oman2.objects[oman2.Akt_Menu][*card1].card.visible = 0;
				oman2.objects[oman2.Akt_Menu][*card2].card.visible = 0;

				char *c = (char *)malloc(sizeof(char));
				sprintf(c, "%d", AktPlayer + 1);
				Players[AktPlayer].ObPlayername->picture = Create_Picture_By_Text(Players[AktPlayer].ObPlayername->picture, Players[AktPlayer].Name, 0);

				AktPlayer++;
				if (AktPlayer == amplayers) AktPlayer = 0;

				sprintf(c, "%d", AktPlayer + 1);
				Players[AktPlayer].ObPlayername->picture = Create_Picture_By_Text(Players[AktPlayer].ObPlayername->picture, Players[AktPlayer].Name, 1);
			}
			*mod = 0;
			break;
		default: break;
		}
		paint_screen(_screen, oman2.objects[oman2.Akt_Menu]);
	}
}

int GameEnd(int amPlayers)
{
	int iwinner;
	int iPairs = 0;
	for (int i = 0; i < amPlayers; i++)
	if (Players[i].FoundPairs > iPairs)
	{
		iwinner = i;
		iPairs = Players[i].FoundPairs;
	}

	int i = 0;
	while (!IS_NULL(oman2.objects[oman2.Akt_Menu][i++]));
	i--;

	char c[100];
	c[0] = '\0';
	strcat(&c, &"The Winner with ");

	char c2[100];
	sprintf(&c2, "%d", iPairs);
	strcat(&c, &c2);

	if (iPairs == 1)
		strcat(&c, &" pair is Player ");
	else
		strcat(&c, &" pairs is Player ");
	
	sprintf(&c2, "%d", iwinner);
	strcat(&c, &c2);

	oman2.objects[oman2.Akt_Menu][i] = O_New_Label(oman2.objects[oman2.Akt_Menu][i], c, 300, 300);

	i++;
	oman2.objects[oman2.Akt_Menu][i] = O_New_Button(oman2.objects[oman2.Akt_Menu][i], "Continue", BContinue, 300, 400);
}

char* concat(char *s1, char *s2)
{
	char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);//+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);

	return result;
}

int init_game(int AmPlayers, struct Card (*stack)[], int AmCards)
{
	oman2 = Load_Objects(oman2, "./resources/game_config.txt");

	FILE *f;
	f = fopen("./resources/config.txt", "r");
		
	if (fscanf(f, "%d %d %d %d", &Gamefield.x, &Gamefield.y, &Gamefield.w, &Gamefield.h) == EOF)
		return FAILED_LOADING_GAMEFIELD;

	int index = 0;
	while (!IS_NULL(oman2.objects[oman2.Akt_Menu][index++]));
	index--;

	int i = 0;
	for (i = 0; i < AmPlayers; i++)
	{
		Players[i].FoundPairs = 0;
		int PPx = 0, PPy = 0;
		if (fscanf(f, "%d %d", &PPx, &PPy) == EOF)
			return FAILED_LOADING_PlAYERPOINTSPOSITION;

		Players[i].ObPlayername = &oman2.objects[0][index++];
		Players[i].ObPlayerPoints = &oman2.objects[0][index++];
		*Players[i].ObPlayername = O_New_Label(*Players[i].ObPlayername, Players[i].Name, PPx, PPy);
		*Players[i].ObPlayerPoints = O_New_Label(*Players[i].ObPlayerPoints, "0 pairs found", PPx, PPy + 30);
	}

	for (i = AmPlayers; i < 8; i++)
		fscanf(f, "%d %d");

	int X, Y;
	if (fscanf(f, "%d %d", &X, &Y) != EOF)
	{
		char *c = (char *)malloc(sizeof(char));
		NuOfRePairs = AmCards / 2;
		sprintf(c, "%d", NuOfRePairs);

		oman2.objects[oman2.Akt_Menu][index] = O_New_Label(oman2.objects[oman2.Akt_Menu][index], concat("Number of remaining pairs: ", c), X, Y);
		
		PosOfReCards = index++;
	}

	int SizeX = (*(*stack)[0].picture).picture->w;
	int SizeY = (*(*stack)[0].picture).picture->h;

	int AmX = Gamefield.w / SizeX;
	int AmY = Gamefield.h / SizeY;

	if (AmX * AmY < AmCards)
		AmCards -= 2 * (int)((AmCards - AmX * AmY + 1) / 2);
	
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
	
	for (int x = 0; x < AmCards; x++)
	{
		if ((*(*stack)[j].picture).picture == NULL)
			break;

		oman2.objects[oman2.Akt_Menu][index].type = TCard; // oman2.objects[oman2.Akt_Menu] is card
		oman2.objects[oman2.Akt_Menu][index].card = (*stack)[j++];
		oman2.objects[oman2.Akt_Menu][index].picture = *oman2.objects[oman2.Akt_Menu][index].card.picture;
		oman2.objects[oman2.Akt_Menu][index].card.picture = &oman2.objects[oman2.Akt_Menu][index].picture;
		oman2.objects[oman2.Akt_Menu][index].enabled = 1;
		oman2.objects[oman2.Akt_Menu][index].x = Gamefield.x + Gamefield.w / AmX * (x % AmX);
		oman2.objects[oman2.Akt_Menu][index].y = Gamefield.y + Gamefield.h / AmY * (int)(x / AmX);
		index++;
	}


	UpdateRemainingCards(AmCards);

	return 0;
}

int UpdateRemainingCards(int AmCards)
{
	char *c = (char *)malloc(sizeof(char));
	NuOfRePairs = AmCards / 2;
	sprintf(c, "%d", NuOfRePairs);

	oman2.objects[oman2.Akt_Menu][PosOfReCards].picture = Create_Picture_By_Text(oman2.objects[oman2.Akt_Menu][PosOfReCards].picture, concat("Number of remaining pairs: ", c), 0);
}