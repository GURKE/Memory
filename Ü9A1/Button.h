#ifndef _BUTTON_H_
#define _BUTTON_H_

#pragma warning( disable : 4996 )

#include "Picture.h"

#define BExit 0
#define BSave 1
#define BStart 2
#define BHighscore 3
#define BContinue 4
#define BMainMenu 5
#define BConfigurations 6
#define BSoundOn 7
#define BSoundOff 8
#define BBackground 9


struct Button
{
	struct Picture Picture;
	struct Picture Clicked_Picture;
	int Clicked;
	int Type; // 0 = exit, 1 = save, 2 = start
};

struct Button New_Button(struct Button b, char Text[]);
int Save_Button(FILE *f, struct Button b);

#endif