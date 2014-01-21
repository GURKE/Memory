#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Button.h"

#pragma warning( disable : 4996 )


struct Button New_Button(struct Button b, char Text[])
{
	b.Clicked = 0;
	b.Picture = Create_Picture_By_Text(b.Picture, Text, 0);
	b.Clicked_Picture = Create_Picture_By_Text(b.Clicked_Picture, Text, 1);

	return b;
}

int Save_Button(FILE *f, struct Button b)
{
	fprintf(f, "%d", b.Type);
}
