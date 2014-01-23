#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#pragma warning( disable : 4996 )

#include "Object.h"

#define Number_Of_Menues 4

#define MAIN_MENU 0
#define HIGH_SCORE 1
#define CONFIGURATIONS 2
#define START_MENU 3

struct Objectmanager
{
	struct Object objects[Number_Of_Menues][ARRAY_LENGTH];
	int Buttons[Number_Of_Menues][ARRAY_LENGTH];
	int NumberOfButtons[Number_Of_Menues];
	int Akt_Menu;
	int Akt_Button;
};

struct Objectmanager Load_Objects(struct Objectmanager oman, FILE *f);
int paint_screen(SDL_Surface *_screen);
int dist2object(int x, int y, int type[], int AmOfTypes);

#endif // !_OBJECTMANAGER_H_
