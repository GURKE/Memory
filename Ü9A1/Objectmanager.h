#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#pragma warning( disable : 4996 )

#include "Object.h"
#include "Generel_Proc.h"

#define Number_Of_Menues 6

#define MMAIN_MENU			0
#define MHIGH_SCORE			1
#define MCONFIGURATIONS		2
#define MSTART_MENU			3
#define MClear_Highscore	4
#define MTooLessCards		5

#define MGAME				0
#define MEXIT				1
#define MGAME_HS			2

struct Objectmanager
{
	struct Object objects[Number_Of_Menues][ARRAY_LENGTH];
	int Buttons[Number_Of_Menues][ARRAY_LENGTH];
	int NumberOfButtons[Number_Of_Menues];
	int Akt_Menu;
	int Akt_Button;
	int Akt_Button_Activated_By_Mouse;
};

struct Objectmanager Load_Objects(struct Objectmanager oman, char Filename[]);
int paint_screen(SDL_Surface *_screen, struct Object objects[]);
int dist2object(struct Object objects[], int x, int y, int type[], int AmOfTypes);
int Change_Menu(struct Objectmanager *oman, SDL_Surface *_screen, int Menu, int *Akt_Button);
int init_cards(struct Objectmanager *oman, struct Pair(*cards)[], char FileName[], struct Picture Background, int Menu, int shuffle, int AmPairs, int onlyForeground);

#endif // !_OBJECTMANAGER_H_
