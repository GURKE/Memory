#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "memory.h"
//#include "Card.h"
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
int init_game(amplayers, Cards, AmPairs, cards, res1080p);
int start_game(int amplayers, struct Object Cards[], int AmCards, struct Picture *BG, int Loading_Game, struct Player _players[], struct Pair(*cards)[], int res1080p);
int Mouse_Clicked(int *mod, int *card1, int *card2, int amCards, SDL_Event event, int amplayers);
int Mouse_Motion(int *mod, int *card1, int *card2, SDL_Event event, int amplayers);
int Save_Game(int amPlayers, struct Pair *cards[]);
int Load_Game();
int UpdateRemainingCards(int AmCards);
struct Player *SortWinner(int AmPlayer);

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

struct Player *GetWinner();

int start_game(int amplayers, struct Object Cards[], int AmCards, struct Picture *BG, int Loading_Game, struct Player _players[], struct Pair(*cards)[], int res1080p)
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
	
	for (int i = 0; i < amplayers; i++)
		Players[i] = _players[i];

	int result = init_game(amplayers, Cards, AmCards, cards, res1080p);
	if (result)
		return result;

	paint_screen(_screen, oman2.objects[oman2.Akt_Menu]);
	
	while (SDL_WaitEvent(&event))
	{
		if (event.type == SDL_QUIT)
			break;
		if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			int result = Mouse_Clicked(mod, card1, card2, AmCards, event, amplayers);
			if (result == GECLOSED || result == GEENDED)
				return result;

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
	int Types[] = { TButton };
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

int Save_Game(int amPlayers, int amCards, struct Pair *cards[])
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

	struct Pair cards[ARRAY_LENGTH];
	//init_cards(&oman2, &cards, "Savegame_Cards.txt", Card_Background, MGAME);

	Card_Background = load_picture(Card_Background, FileName);

	//start_game(amPlayers, cards, amCards, GameSizeX, GameSizeY, &Card_Background, 1, Players);

	return 0;
}

int Mouse_Clicked(int *mod, int *card1, int *card2, int amCards, SDL_Event event, int amplayers)
{
	int Types[] = { TCard, TButton };
	int actcard = dist2object(oman2.objects[oman2.Akt_Menu], event.button.x, event.button.y, Types, 2);

	if (oman2.objects[oman2.Akt_Menu][actcard].type == TButton) // Button
	{
		switch (oman2.objects[oman2.Akt_Menu][actcard].button.Type) // Beenden
		{
		case BGameExit: Change_Menu(&oman2, _screen, MEXIT, &oman2.Akt_Button); return 0;
		case BBack: Change_Menu(&oman2, _screen, MGAME, &oman2.Akt_Button); return 0;
		case BGameExitConfirmation: return GECLOSED;
		case BContinue: return GEENDED;
		case BSave: Save_Game(amplayers, amCards, 0); break;
		case BLoad: Load_Game(); break;
		default:	break;
		}
	}
	else  // Card
	{
		switch (*mod)
		{
		case 0:	case 1: // tap card
			if (actcard > -1 && oman2.objects[oman2.Akt_Menu][actcard].enabled && !oman2.objects[oman2.Akt_Menu][actcard].button.Clicked)
			{
				if (*mod == 0)
					*card1 = actcard;
				else
					*card2 = actcard;

				oman2.objects[oman2.Akt_Menu][actcard].button.Clicked = 1;
				(*mod)++;
			}
			break;

		case 2: // Remove them or move them back
			if (amplayers == 1)
				Players[AktPlayer].FoundPairs++;

			if (oman2.objects[oman2.Akt_Menu][*card1].button.Value == oman2.objects[oman2.Akt_Menu][*card2].button.Value)
			{ // Pair found
				oman2.objects[oman2.Akt_Menu][*card1].enabled = 0;
				oman2.objects[oman2.Akt_Menu][*card2].enabled = 0;

				if (amplayers > 1)	
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
				oman2.objects[oman2.Akt_Menu][*card1].button.Clicked = 0;
				oman2.objects[oman2.Akt_Menu][*card2].button.Clicked = 0;

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
	return 0;
}

int GameEnd(int amPlayers)
{
	struct Player *Winner = SortWinner(amPlayers);

	Change_Menu(&oman2, _screen, MGAME_HS, &oman2.Akt_Button);

	char Highscoreitemfilename[ARRAY_LENGTH];

	// Searches the last item and searches the filename for the Highscoreitem
	int index = 0;
	int ItemFound = 0;
	while (!IS_NULL(oman2.objects[oman2.Akt_Menu][index]))
	{
		if (oman2.objects[oman2.Akt_Menu][index].type == THighscoreitem) 
		{
			if (ItemFound)
				oman2.objects[oman2.Akt_Menu][index].enabled = 0; // Disable old Highscoreitems
			else
			{
				strcpy(Highscoreitemfilename, oman2.objects[oman2.Akt_Menu][index].picture.filename);
				ItemFound = 1;
			}
		}
		index++;
	}
	index--;

	int j = 0;
	int rank = 1;
	// List the winner
	for (j = 0; j < amPlayers; j++)
	{
		if (Highscoreitemfilename[0] != '\0') 
		{
			oman2.objects[oman2.Akt_Menu][index] = O_New_Object(oman2.objects[oman2.Akt_Menu][index], 248, 166 + j * 49, THighscoreitem, Highscoreitemfilename, "", 0, 0);
			index++;
		}

		if (j == 0 || Winner[j].FoundPairs != Winner[j - 1].FoundPairs) // No need for an new rank number
		{
			char c[2];
			sprintf(&c, "%d", rank);

			oman2.objects[oman2.Akt_Menu][index] = O_New_Label(oman2.objects[oman2.Akt_Menu][index], c, 264, 181 + j * 49);
			oman2.objects[oman2.Akt_Menu][index].type = THighscoreitem;
			index++;
			rank++;
		}

		oman2.objects[oman2.Akt_Menu][index] = O_New_Label(oman2.objects[oman2.Akt_Menu][index], Winner[j].Name, 364, 181 + j * 49);
		oman2.objects[oman2.Akt_Menu][index].type = THighscoreitem; 
		index++;

		char c[10];
		sprintf(&c, "%d", Winner[j].FoundPairs);
		oman2.objects[oman2.Akt_Menu][index] = O_New_Label(oman2.objects[oman2.Akt_Menu][index], c, 845, 181 + j * 49);
		oman2.objects[oman2.Akt_Menu][index].type = THighscoreitem; 
		index++;
	}
}

struct Player *GetWinner()
{
	return Players;
}

int init_game(int AmPlayers, struct Object Cards[], int AmPairs, struct Pair(*cards)[], int res1080p)
{
	oman2.Akt_Menu = MGAME;
	
	int i = 0;
	int j = 0;
	for (i = 0; i < Number_Of_Menues; i++)
	for (j = 0; j < ARRAY_LENGTH; j++)
		oman2.objects[i][j].picture.picture = NULL;

	FILE *f;

	if (res1080p)
	{
		oman2 = Load_Objects(oman2, "./resources/game_config_1080p.txt");
		_screen = SDL_SetVideoMode(1920, 1080, SCREEN_BPP, SDL_HWSURFACE | SDL_FULLSCREEN);
		f = fopen("./resources/config_1080p.txt", "r");
	}
	else
	{
		oman2 = Load_Objects(oman2, "./resources/game_config.txt");
		f = fopen("./resources/config.txt", "r");
	}
	
	if (fscanf(f, "%d %d %d %d", &Gamefield.x, &Gamefield.y, &Gamefield.w, &Gamefield.h) == EOF)
		return FAILED_LOADING_GAMEFIELD;

	int index = 0;
	while (!IS_NULL(oman2.objects[oman2.Akt_Menu][index]))
	{
		if (oman2.objects[oman2.Akt_Menu][index].type == TCard) // Removes the old cards after the first game
			oman2.objects[oman2.Akt_Menu][index].picture.picture = NULL;
		index++;
	}
	
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
		NuOfRePairs = AmPairs / 2;
		sprintf(c, "%d", NuOfRePairs);

		oman2.objects[oman2.Akt_Menu][index] = O_New_Label(oman2.objects[oman2.Akt_Menu][index], concat("Number of remaining pairs: ", c), X, Y);
		
		PosOfReCards = index++;
	}

	int SizeX = Cards[0].picture.picture->w;
	int SizeY = Cards[0].picture.picture->h;

	int AmX = Gamefield.w / (SizeX + 5);
	int AmY = Gamefield.h / (SizeY + 5);

	if (AmX * AmY < AmPairs * 2)
		AmPairs -= (int)((2 * AmPairs - AmX * AmY + 1) / 2);
	
	AktPlayer = rand() % AmPlayers;

	Players[AktPlayer].ObPlayername->picture = Create_Picture_By_Text(Players[AktPlayer].ObPlayername->picture, Players[AktPlayer].Name, 1);

	// put random pairs into the deck

	struct Pair RealStack[ARRAY_LENGTH];
	int UsedCards = 0;
		
	while (UsedCards < AmPairs)
	{
		int k = rand() % AmPairs;  /* random variable modulo remaining cards */
		int AlreadyInUse = 0;
		for (int i = 0; i < UsedCards; i++)
		if (RealStack[i].id == (*cards)[Cards[k].PairID].id)
		{
			AlreadyInUse = 1;
			break;
		}

		if (!AlreadyInUse)
			RealStack[UsedCards++] = (*cards)[Cards[k].PairID];
	}

	// Draw Cards on gamefield
	AmPairs = init_cards(&oman2, &RealStack, "", Card_Background, MGAME, 1, AmPairs, 0);
	
	j = 0;
	
	for (int x = 0; x < AmPairs * 2; x++)
	{
		if (oman2.objects[oman2.Akt_Menu][index].picture.picture == NULL)
			break;		

		oman2.objects[oman2.Akt_Menu][index].enabled = 1;
		oman2.objects[oman2.Akt_Menu][index].x = Gamefield.x + Gamefield.w / AmX * (x % AmX);
		oman2.objects[oman2.Akt_Menu][index].y = Gamefield.y + Gamefield.h / AmY * (int)(x / AmX);
		index++;
	}


	UpdateRemainingCards(AmPairs);

	return 0;
}

int UpdateRemainingCards(int AmCards)
{
	char *c = (char *)malloc(sizeof(char));
	NuOfRePairs = AmCards / 2;
	sprintf(c, "%d", NuOfRePairs);

	oman2.objects[oman2.Akt_Menu][PosOfReCards].picture = Create_Picture_By_Text(oman2.objects[oman2.Akt_Menu][PosOfReCards].picture, concat("Number of remaining pairs: ", c), 0);
}

struct Player *SortWinner(int AmPlayer)
{
	// Sort Winner
	int i = 0;
	struct Player *Winner = (struct Player *)malloc(AmPlayer * sizeof(struct Player));
	for (i = 0; i < AmPlayer; i++)
	{
		Winner[i].Name[0] = '\0';
		int j = 0; // Search the position in list
		while (Winner[j].FoundPairs > Players[i].FoundPairs && Winner[j].Name != '\0') j++;
		int k = i; // Push the following items 1 down
		for (; k > j && Winner[k].Name != '\0'; k--)
			Winner[k] = Winner[k - 1];

		Winner[j] = Players[i];
	}

	return Winner;
}