#ifndef _BUTTON_H_
#define _BUTTON_H_

#pragma warning( disable : 4996 )

#include "Picture.h"

struct Button
{
	struct Picture *Picture;
	struct Picture *Clicked_Picture;
	struct Picture Text_Picture;
	int Clicked;
	int Type;
};


#endif