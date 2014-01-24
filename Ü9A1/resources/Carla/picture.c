#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include "Picture.h"


SDL_Color textColor = { 255, 255, 255 };
SDL_Color textColorH = { 200, 200, 0 };



#define COLOR_KEY_R 0
#define COLOR_KEY_G 0
#define COLOR_KEY_B 0


struct Picture load_picture(struct Picture p, char Filename[100])
{
        strncpy(p.filename);
        return;
}
