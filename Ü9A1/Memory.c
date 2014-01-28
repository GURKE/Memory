#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "memory.h"
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


struct Objectmanager oman;

int Print_Cards_In_Top_List();
int Print_Cards_In_Bottom_List();

int init_card_background();
int init_menu();
int Paint_Highscore();


int Key_Clicked(SDL_Event event);

int Speaker = -1;
int SoundOn = 1;

int Site_Selected = -1; // ID of the picture of side_selected

int Act_Lecture = 1;
int Offset_Up_X = 0;
int Offset_Down_X = 0;

#define AmLections 3

struct Object *AvaiCards[AmLections][ARRAY_LENGTH];
int AvaiCardsIndex[AmLections] = { 0, 0, 0 };
struct Object *UsedCards[ARRAY_LENGTH];
int UsedCardsIndex = 0;

struct Pair cards[ARRAY_LENGTH];
int CardsIDs[ARRAY_LENGTH];
int Max_Show_Cards = 5;

int Act_Song = 0;
int Songs_to_play[ARRAY_LENGTH];
char Songs[ARRAY_LENGTH][ARRAY_LENGTH];

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL konnte nicht initialisiert werden:  %s\n", SDL_GetError());
		return INITIALIZATION_FAILURE;
	}

	_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_FULLSCREEN);
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

	srand(time(NULL)); /* start random number generater */

	// Loads the Font
	font = TTF_OpenFont("./resources/lazy.ttf", 28);
	if (font == NULL)
	{
		fprintf(stderr, "SDL_TTF konnte nicht initialisiert werden:  %s\n", TTF_GetError());
		return FONT_LOAD_FAILURE;
	}

	_bg_color = SDL_MapRGB(_screen->format, 0, 0, 0); // Sets the backgroundcolor to black
		
	init_menu();
	init_card_background();
	int AmCards = init_cards(&oman, &cards[0], "./resources/cards/cardlist.txt", card_background[Akt_Background], MSTART_MENU, 0, -1, 1);
	init_Gamestart();
	init_Music();

	SDL_Event event;
	int AmPlayers = 1;
	int act_Highscore = 0;

	int Res1080p = 0;
	
	int Act_Textbox = -1;

	oman.objects[MSTART_MENU][38].enabled = 0;

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
				int Types[] = { TCard, TButton, TButtonWithFrame, TTextbox };
				actobject = dist2object(oman.objects[oman.Akt_Menu], event.button.x, event.button.y, Types, 4);
			}

			if (actobject > -1)
			{
				if (oman.objects[oman.Akt_Menu][actobject].type == TTextbox)
				{
					Act_Textbox = actobject;
					oman.objects[oman.Akt_Menu][Act_Textbox].label.Text[0] = '\0';
					oman.objects[oman.Akt_Menu][Act_Textbox].picture = Create_Picture_By_Text(oman.objects[oman.Akt_Menu][Act_Textbox].picture, oman.objects[oman.Akt_Menu][Act_Textbox].label.Text, 0);
					paint_screen(_screen, oman.objects[oman.Akt_Menu]);
				}
				else
				{
					Act_Textbox = -1;

					if (oman.objects[oman.Akt_Menu][actobject].button.Type == BStart) // Start game
					{
						Change_Menu(&oman, _screen, MSTART_MENU, &oman.Akt_Button);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BHighscore)
					{
						act_Highscore = oman.objects[oman.Akt_Menu][actobject].button.Value;
						Paint_Highscore(act_Highscore);

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
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSet1080p)
					{
						Res1080p = 1;
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
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSwitchSong)
					{
						Act_Song = oman.objects[oman.Akt_Menu][actobject].button.Value;
						SwitchSound(1);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BUseCard)
					{
						AddCardToList(oman.objects[oman.Akt_Menu][actobject].button.Value);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BSwitchLecture)
					{
						for (int i = Offset_Up_X; i < Max_Show_Cards && i < AvaiCardsIndex[Act_Lecture]; i++)
							AvaiCards[Act_Lecture][i]->enabled = 0;

						Offset_Up_X = 0;
						Act_Lecture = oman.objects[oman.Akt_Menu][actobject].button.Value;
						Print_Cards_In_Top_List();
						paint_screen(_screen, oman.objects[MSTART_MENU]);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BClearHighscore)
					{
						Change_Menu(&oman, _screen, MClear_Highscore, &oman.Akt_Button);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BGameExitConfirmation)
					{
						char c[ARRAY_LENGTH];
						c[0] = '\0';
						strcat(&c, "./resources/Highscores/highscore_");
						char c2[2];
						sprintf(&c2, "%d", act_Highscore);
						strcat(&c, &c2);
						strcat(&c, ".txt");
						FILE *f = fopen(c, "w");
						fprintf(f, "");

						Paint_Highscore(act_Highscore);

						Change_Menu(&oman, _screen, MHIGH_SCORE, &oman.Akt_Button);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BBack)
					{
						Change_Menu(&oman, _screen, MHIGH_SCORE, &oman.Akt_Button);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BOk)
					{
						Change_Menu(&oman, _screen, MSTART_MENU, &oman.Akt_Button);
					}
					else if (oman.objects[oman.Akt_Menu][actobject].button.Type == BStartGame)
					{
						if (UsedCardsIndex > 0)
						{
							struct Player Players[8];

							for (int i = 0; !IS_NULL(oman.objects[oman.Akt_Menu][i]); i++)
							if (oman.objects[oman.Akt_Menu][i].type == TTextbox)
							{
								if (oman.objects[oman.Akt_Menu][i].label.Textboxvalue > -1 && oman.objects[oman.Akt_Menu][i].label.Textboxvalue < 8)
									strcpy(Players[oman.objects[oman.Akt_Menu][i].label.Textboxvalue].Name, oman.objects[oman.Akt_Menu][i].label.Text);
							}

							int result = start_game(AmPlayers, *UsedCards, UsedCardsIndex, &card_background[Akt_Background], 0, Players, &cards, Res1080p);
							_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE|SDL_FULLSCREEN);
							if (result == GEENDED) // Finishes Playing
							{
								Load_Highscore(AmPlayers);
								add_Highscoreitems(GetWinner(), AmPlayers, AmCards);
								print_Highscore(AmPlayers);
								Paint_Highscore(AmPlayers);
								act_Highscore = AmPlayers;
								Change_Menu(&oman, _screen, MHIGH_SCORE, &oman.Akt_Button);
							}
							else if (result == GECLOSED)
								Change_Menu(&oman, _screen, MMAIN_MENU, &oman.Akt_Button);
						}
						else // Too less cards in use
						{
							Change_Menu(&oman, _screen, MTooLessCards, &oman.Akt_Button);
						}
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
			Key_Clicked(event, Act_Textbox);
		}
	}

	SDL_Quit();

	return 0;
}

int init_Music()
{
	FILE *f = fopen("./resources/music/backgroundmusic.txt", "r");

	int i = 0;
	while (freadString(f, &Songs[i], "#\0", 100) != 1) freadString(f, &Songs[i++ + 1], "\n", 100);

	PlaySoundA(Songs[0], NULL, SND_LOOP | SND_ASYNC);
}

int Move_Cards(int value, int AmCards)
{
	switch (value)
	{
	case 1: 
		if (Offset_Up_X > 0)
		{
			if (Offset_Up_X + Max_Show_Cards < AvaiCardsIndex[Act_Lecture])
				AvaiCards[Act_Lecture][Offset_Up_X + Max_Show_Cards]->enabled = 0;

			Offset_Up_X--;
			Print_Cards_In_Top_List();
			paint_screen(_screen, oman.objects[MSTART_MENU]);
		}
		break;
	case 2:
		if (Offset_Up_X < AvaiCardsIndex[Act_Lecture] - Max_Show_Cards)
		{
			if (Offset_Up_X > AvaiCardsIndex)
				AvaiCards[Act_Lecture][Offset_Up_X]->enabled = 0;
			
			Offset_Up_X++;
			Print_Cards_In_Top_List();
			paint_screen(_screen, oman.objects[MSTART_MENU]);
		}
		break;
	case 3:
		if (Offset_Down_X > 0)
		{
			if (Offset_Down_X + Max_Show_Cards < UsedCardsIndex)
				UsedCards[Offset_Down_X + Max_Show_Cards]->enabled = 0;

			Offset_Down_X--;
			Print_Cards_In_Bottom_List();
			paint_screen(_screen, oman.objects[MSTART_MENU]);
		}
		break;
	case 4:
		if (Offset_Down_X < UsedCardsIndex - Max_Show_Cards)
		{
			if (Offset_Down_X + Max_Show_Cards < UsedCardsIndex)
				UsedCards[Offset_Down_X + Max_Show_Cards]->enabled = 0;

			Offset_Down_X++;
			Print_Cards_In_Bottom_List();
			paint_screen(_screen, oman.objects[MSTART_MENU]);
		}
		break; 
	default: break;

	}
}

int AddCardToList(int value)
{
	for (int i = 0; i < Max_Show_Cards; i++)
	{
		if (i + Offset_Up_X < AvaiCardsIndex[Act_Lecture] && AvaiCards[Act_Lecture][i + Offset_Up_X]->button.Value == value)
		{
			UsedCards[UsedCardsIndex] = AvaiCards[Act_Lecture][i + Offset_Up_X];
			UsedCards[UsedCardsIndex]->enabled = 0;
			for (int j = i + Offset_Up_X; j < AvaiCardsIndex[Act_Lecture] - 1; j++)
				AvaiCards[Act_Lecture][j] = AvaiCards[Act_Lecture][j + 1];

			AvaiCardsIndex[Act_Lecture]--;
			UsedCardsIndex++;
			break;
		}
		else if (i + Offset_Down_X < UsedCardsIndex && UsedCards[i + Offset_Down_X]->button.Value == value)
		{
			AvaiCards[Act_Lecture][AvaiCardsIndex[Act_Lecture]] = UsedCards[i + Offset_Down_X];
			AvaiCards[Act_Lecture][AvaiCardsIndex[Act_Lecture]]->enabled = 0;
			for (int j = i + Offset_Down_X; j < UsedCardsIndex - 1; j++)
				UsedCards[j] = UsedCards[j + 1];

			AvaiCardsIndex[Act_Lecture]++;
			UsedCardsIndex--;
			break;
		}
	}

	Print_Cards_In_Top_List();
	Print_Cards_In_Bottom_List();
	paint_screen(_screen, oman.objects[MSTART_MENU]);
}

int init_Gamestart()
{
	int i;
	for (i = 0; cards[i].id > -1; i++)
		AvaiCards[cards[i].topic][AvaiCardsIndex[cards[i].topic]++] = &oman.objects[MSTART_MENU][cards[i].ObjectId];

	AvaiCards[cards[i - 1].topic][AvaiCardsIndex[cards[i - 1].topic]-- - 1] = NULL;

	Act_Lecture = 0;

	Print_Cards_In_Top_List();
	paint_screen(_screen, oman.objects[MSTART_MENU]);
}

int Contains(int Array[], int number, int ArrayLength)
{
	for (int i = 0; i < ArrayLength; i++)
	{
		if (Array[i] == number)
			return i + 1;
	}

	return 0;
}

int Print_Cards_In_Bottom_List()
{
	int i = 0;

	for (i = Offset_Down_X; i < Max_Show_Cards + Offset_Down_X && i < UsedCardsIndex; i++)
	{
		UsedCards[i]->x = 100 + 220 * (i - Offset_Down_X);
		UsedCards[i]->y = 486;
		UsedCards[i]->enabled = 1;
		UsedCards[i]->button.Clicked = 1;
	}
}

int Print_Cards_In_Top_List()
{
	int i = 0;

	for (i = Offset_Up_X; i < Max_Show_Cards + Offset_Up_X && i < AvaiCardsIndex[Act_Lecture]; i++)
	{
		AvaiCards[Act_Lecture][i]->x = 100 + 220 * (i - Offset_Up_X);
		AvaiCards[Act_Lecture][i]->y = 268;
		AvaiCards[Act_Lecture][i]->enabled = 1;
		AvaiCards[Act_Lecture][i]->button.Clicked = 1;
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
		PlaySoundA(Songs[Act_Song], NULL, SND_LOOP | SND_ASYNC);
		SoundOn = 1;
		if (Speaker > -1)
			oman.objects[oman.Akt_Menu][Speaker].button.Clicked = 0;
	}
	paint_screen(_screen, oman.objects[oman.Akt_Menu]);
}

int Key_Clicked(SDL_Event event, int Act_Textbox)
{
	if (Act_Textbox > -1)
	{
		char c[2];
		if (event.key.keysym.sym == SDLK_BACKSPACE)
		{
			int i;
			for (i = 0; oman.objects[oman.Akt_Menu][Act_Textbox].label.Text[i] != '\0'; i++);
			if ( i > 0)
				oman.objects[oman.Akt_Menu][Act_Textbox].label.Text[i - 1] = '\0';
		}
		else
		{
			if (event.key.keysym.sym < SDLK_a || event.key.keysym.sym > SDLK_z)
				return 0;
			if (event.key.keysym.mod == KMOD_LSHIFT)
				sprintf(&c, "%c", (char)(event.key.keysym.sym - 32));
			else
				sprintf(&c, "%c", (char)event.key.keysym.sym);
			strcat(&oman.objects[oman.Akt_Menu][Act_Textbox].label.Text, &c);
		}

		oman.objects[oman.Akt_Menu][Act_Textbox].picture = Create_Picture_By_Text(oman.objects[oman.Akt_Menu][Act_Textbox].picture, oman.objects[oman.Akt_Menu][Act_Textbox].label.Text, 0);
		paint_screen(_screen, oman.objects[oman.Akt_Menu]);
	}
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
