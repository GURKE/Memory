#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Button.h"

#pragma warning( disable : 4996 )


struct Button New_Button(struct Button b, struct Picture *Pic_Button, struct Picture *Pic_Button_Clicked, struct Picture Text_Picture)
{
	b.Picture = Pic_Button;
	b.Clicked_Picture = Pic_Button_Clicked;
	b.Text_Picture = Text_Picture;
	b.Clicked = 0;
	b.x = 0;
	b.y = 0;

	return b;
}