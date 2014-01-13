#ifndef _BUTTON_H_
#define _BUTTON_H_

#pragma warning( disable : 4996 )

#include "Picture.h"

struct Button
{
	struct Picture *Picture;
	struct Picture *Clicked_Picture;
	struct Picture Text_Picture;
	int x;
	int y;
	int Clicked;
	int Type; // 0 = exit, 1 = save, 2 = start
};

struct Button New_Button(struct Button b, struct Picture *Pic_Button, struct Picture *Pic_Button_Clicked, struct Picture Text_Picture);

#endif