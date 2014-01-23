#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "Card.h"

#pragma warning( disable : 4996 )

#define INITIALIZATION_FAILURE						-1
#define NO_SCREEN									-2
#define FAILED_LOADING_GAME_OBJECTS					-4
#define TOO_MANY_PLAYERS							-5
#define FONT_LOAD_FAILURE							-6
#define FAILED_LOADING_GAMEFIELD					-7
#define FAILED_LOADING_PlAYERPOINTSPOSITION			-8
#define FAILED_LOADING_PIC_BUTTON_ADDRESS			-9
#define FAILED_LOADING_PIC_BUTTON_CLICKED_ADDRESS	-10
#define	FAILED_LOADING_GAME							-12
#define FAILED_LOADING_SAVEGAME						-14

#define ARRAY_LENGTH	1000

/** further definitions **/
#define EXTENDED_VERSION

SDL_Surface *_screen;

int start_game(int amplayers, struct Card (*stack)[], int AmCards, int SizeX, int SizeY, struct Picture *BG, int Loading_Game);

#endif