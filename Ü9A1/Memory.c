#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "memory.h"
//#include "Card.h"
#include "Picture.h"
#include "Objectmanager.h"
#include "Highscore.h"
#include "Object.h"
#include "Player.h"
#include "Generel_Proc.h"

#pragma warning( disable : 4996 )
#pragma comment(lib, "winmm.lib") // For the sound

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

struct Picture card_background[ARRAY_LENGTH];
int Akt_Background = 0; // The selected Background
int Card_Frame_ID = 0; // ID of the Card_Frame_ID bmp


struct Pair cards[ARRAY_LENGTH];

int UsedPairs[40];

struct Objectmanager oman;

int Print_Cards_In_Top_List(int Lecture, int Offset_X);
int init_card_background();
int init_menu();
int Paint_Highscore();


int Key_Clicked(SDL_Event event);

int Speaker = -1;
int SoundOn = 1;

int Site_Selected = -1; // ID of the picture of side_selected

int Act_Lecture = 1;
int Offset_Up_X = 0;

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


	init_menu();
	init_card_background();
	int AmCards = init_cards(&oman, &cards[0], "./resources/cards/cardlist.txt", card_background[Akt_Background], MSTART_MENU, 0, -1);

	SDL_Event event;
	int AmPlayers = 1;

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
				int Types[] = { TCard, TButton, 6 };
				actobject = dist2object(oman.objects[oman.Akt_Menu], event.button.x, event.button.y, Types, 2);
			}

			if (actobject > -1)
			{
				if (oman.objects[oman.Akt_Menu][actobject].button.Type == BStart) // Start game
				{
					init_Gamestart();

					Change_Menu(&oman, _screen, MSTART_MENU, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BHighscore)
				{
					Paint_Highscore(oman.objects[oman.Akt_Menu][actobject].button.Value);

					Change_Menu(&oman, _screen, MHIGH_SCORE, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BMainMenu)
				{
					Change_Menu(&oman, _screen, MMAIN_MENU, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BConfigurations)
				{
					init_options();
					Change_Menu(&oman, _screen, MCONFIGURATIONS, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSoundOn)
				{
					SwitchSound(1);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSoundOff)
				{
					SwitchSound(0);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BExit)
				{
					return 0;
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BArrow)
                {
					Move_Cards(oman.objects[oman.Akt_Menu][actobject].button.Value, AmCards);
                }
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSwitchBackground)
				{
					Akt_Background = oman.objects[oman.Akt_Menu][actobject].button.Value;
					oman.objects[oman.Akt_Menu][Card_Frame_ID].x = 210 - 7 + Akt_Background * 220;
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSwitchSound)
				{
					SwitchSound(-1);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BUseCard)
				{
					AddCardToList(oman.objects[oman.Akt_Menu][actobject].button.Value);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BStartGame)
				{
					struct Player Players[8];

					strcpy(Players[0].Name, "Carl");
					strcpy(Players[1].Name, "Peter");
					strcpy(Players[2].Name, "Heinz");
					strcpy(Players[3].Name, "Klaus");
					strcpy(Players[4].Name, "Gilbert");
					strcpy(Players[5].Name, "Emil");
					strcpy(Players[6].Name, "Rudolf");
					strcpy(Players[7].Name, "Haxebuchs");

					int AmCards = 20;

					int result = start_game(AmPlayers, cards, AmCards, &card_background[Akt_Background], 0, Players);
					if (result == GEENDED) // Finishes Playi	ng
					{
						Load_Highscore(AmPlayers);
						add_Highscoreitems(GetWinner(), AmPlayers, AmCards);
						print_Highscore(AmPlayers);
						Paint_Highscore(AmPlayers);
						Change_Menu(&oman, _screen, MHIGH_SCORE, &oman.Akt_Button);
					}
					else if (result == GECLOSED)
						Change_Menu(&oman, _screen, MMAIN_MENU, &oman.Akt_Button);
				}
				else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSwitchAmOfPlayer)
				{
					int i = 0;
					AmPlayers = oman.objects[MSTART_MENU][actobject].button.Value;
					while (!IS_NULL(oman.objects[MSTART_MENU][i]))
					{
						if (oman.objects[MSTART_MENU][i].type == TSiteSelected)
						{
							oman.objects[MSTART_MENU][i].x = 52 + (oman.objects[MSTART_MENU][actobject].button.Value - 1) * 49; break;
						}
						i++;
					}
					paint_screen(_screen, oman.objects[MSTART_MENU]);
				}
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			// Check for the button
			int Types[] = { 2, 6 };
			int newbutton = dist2object(oman.objects[oman.Akt_Menu], event.button.x, event.button.y, Types, 2);
			if (oman.objects[oman.Akt_Menu][newbutton].button.Value > -1) // -1 -> dont react on mouse
			{
				if (newbutton > -1) // Mouse is above the object
				{
					if (newbutton != oman.Akt_Button)
					{
						if (oman.Akt_Button > -1)
						{ // Reset buttons which where selected before
							oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 0;
							oman.objects[oman.Akt_Menu][newbutton].button.Clicked = 0;
						}

						oman.objects[oman.Akt_Menu][newbutton].button.Clicked = 1;
						oman.Akt_Button = newbutton;
						oman.Akt_Button_Activated_By_Mouse = 1;
					}
				}
				else if (oman.Akt_Button > -1 && oman.Akt_Button_Activated_By_Mouse) // Reset clicked button, after mouse left it
				{
					oman.objects[oman.Akt_Menu][oman.Akt_Button].button.Clicked = 0;
					oman.Akt_Button = -1;
				}
				paint_screen(_screen, oman.objects[oman.Akt_Menu]);
			}
		}
		else if (event.type == SDL_KEYDOWN)
		{
			Key_Clicked(event);
		}
	}

	SDL_Quit();

	return 0;
}

int Move_Cards(int value, int AmCards)
{
	switch (value)
	{
	case 1:
		if (Offset_Up_X > 0)
		{
			Offset_Up_X--;
			Print_Cards_In_Top_List(Act_Lecture, Offset_Up_X);
		}
		break;
	case 2:
		if (Offset_Up_X < AmCards - 5)
		{
			Offset_Up_X++;
			Print_Cards_In_Top_List(Act_Lecture, Offset_Up_X);
		}
		break;
	case 3: break;
	case 4: break;
	default: break;

	}
}

int AddCardToList(int id)
{
	int i = 0;
	while (UsedPairs != NULL)i++;
	UsedPairs[i] = id;

	Print_Cards_In_Top_List(Act_Lecture, Offset_Up_X);
}

int init_Gamestart()
{
	Print_Cards_In_Top_List(1, 0);
}

int Contains(int Array[], int number, int ArrayLength)
{
	for (int i = 0; i < ArrayLength; i++)
	{
		if (Array[i] == number)
			return i;
	}

	return 0;
}

int Print_Cards_In_Top_List(int Lecture, int Offset_X)
{
	int i = 0;
	int CardIDs[5];

	while (i < 5 && cards[i].id != -1)
	{
		if (cards[i].topic == Lecture && !Contains(UsedPairs, cards[i].id, 40))
			CardIDs[i] = cards[i++].id;
	}

	for (int j = 0; !IS_NULL(oman.objects[MSTART_MENU][j]); j++)
	{
		for (i = 0; i < 5; i++)
		{
			if (oman.objects[MSTART_MENU][j].button.Value == CardIDs[i] && oman.objects[MSTART_MENU][j].type == TCard)
			{
				oman.objects[MSTART_MENU][j].x = 100 + 220 * i;
				oman.objects[MSTART_MENU][j].y = 260;
				oman.objects[MSTART_MENU][j].enabled = 1;
				oman.objects[MSTART_MENU][j].button.Clicked = 1;
			}
		}
	}
}

int init_options()
{
	int i = 0;
	while (!IS_NULL(oman.objects[MCONFIGURATIONS][i]))
	{
		if (oman.objects[MCONFIGURATIONS][i].button.Type == BSwitchSound)
			Speaker = i;
		else if (oman.objects[MCONFIGURATIONS][i].type == TCardFrameChosen)
			Card_Frame_ID = i;
		i++;
	}

	oman.objects[oman.Akt_Menu][Card_Frame_ID].x = 210 - 7 + Akt_Background * 220;

	i--;
}

int SwitchSound(int Switch)
{
	if ((SoundOn && Switch == -1 ) || Switch == 0)
	{
		PlaySound(NULL, NULL, SND_LOOP | SND_ASYNC);
		SoundOn = 0;
		if (Speaker > -1)
			oman.objects[oman.Akt_Menu][Speaker].button.Clicked = 1;
	}
	else
	{
		PlaySound(L"./resources/music/Bioweapon - Heretic.wav", NULL, SND_LOOP | SND_ASYNC);
		SoundOn = 1;
		if (Speaker > -1)
			oman.objects[oman.Akt_Menu][Speaker].button.Clicked = 0;
	}
	paint_screen(_screen, oman.objects[oman.Akt_Menu]);
}

int Key_Clicked(SDL_Event event)
{
	if (event.key.keysym.sym == SDLK_DOWN)
	{
		if (oman.Akt_Button > -1)
			oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 0;

		oman.Akt_Button++;
		if (oman.Akt_Button >= oman.NumberOfButtons[oman.Akt_Menu])
			oman.Akt_Button = 0;

		oman.Akt_Button_Activated_By_Mouse = 0;

		oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 1;

		paint_screen(_screen, oman.objects[oman.Akt_Menu]);
	}
	else if (event.key.keysym.sym == SDLK_UP)
	{
		if (oman.Akt_Button > -1)
			oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 0;

		oman.Akt_Button--;
		if (oman.Akt_Button < 0)
			oman.Akt_Button = oman.NumberOfButtons[oman.Akt_Menu] - 1;

		oman.Akt_Button_Activated_By_Mouse = 0;

		oman.objects[oman.Akt_Menu][oman.Buttons[oman.Akt_Menu][oman.Akt_Button]].button.Clicked = 1;

		paint_screen(_screen, oman.objects[oman.Akt_Menu]);
	}
}

int Paint_Highscore(int AmPlayer)
{
	int i = 0;
	while (!IS_NULL(oman.objects[MHIGH_SCORE][i]))
	{
		if (oman.objects[MHIGH_SCORE][i].type == THighscoreitem)
			oman.objects[MHIGH_SCORE][i].enabled = 0;
		if (oman.objects[MHIGH_SCORE][i].type == TSiteSelected)
			oman.objects[MHIGH_SCORE][i].x = 492 + (AmPlayer - 1) * 30;
		i++;
	}

	struct Object *o = GetHighscoreItems(AmPlayer);
	for (int j = 0; j < HS_LENGTH * 2; j++)
		oman.objects[MHIGH_SCORE][i++] = *(o++);

	return -1;
}

int init_menu()
{
	// Reset objects
	for (int j = 0; j < Number_Of_Menues; j++)
	for (int i = 0; i < ARRAY_LENGTH; i++)
		oman.objects[j][i].picture.picture = NULL;

	// Load the different Menues
	oman = Load_Objects(oman, "./resources/menu_config.txt");

	oman.Akt_Button_Activated_By_Mouse = 0;
	oman.Akt_Button = -1;

	paint_screen(_screen, oman.objects[oman.Akt_Menu]);
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
