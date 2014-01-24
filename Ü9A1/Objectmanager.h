#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#pragma warning( disable : 4996 )

#include "Object.h"

#define Number_Of_Menues 5

#define MAIN_MENU		0
#define HIGH_SCORE		1
#define CONFIGURATIONS	2
#define START_MENU		3
#define EXIT			4

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
<<<<<<< HEAD
int paint_screen(SDL_Surface *_screen);
int dist2object(int x, int y, int type[], int AmOfTypes);
=======
int paint_screen(SDL_Surface *_screen, struct Object objects[]);
int dist2object(struct Object objects[], int x, int y, int type[], int AmOfTypes);
int Change_Menu(struct Objectmanager *oman, SDL_Surface *_screen, int Menu, int *Akt_Button);
>>>>>>> Optimized Menu

#endif // !_OBJECTMANAGER_H_
