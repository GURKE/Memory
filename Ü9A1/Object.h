#ifndef _OBJECT_H_
#define _OBJECT_H_

//#include "Card.h"
#include "Picture.h"
#include "Button.h"
#include "Label.h"
#include "Pair.h"

#pragma warning( disable : 4996 )

#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT	1024
#define SCREEN_BPP      32

Uint32 _bg_color;
struct Picture Card_Background;

#define THard						0
#define TCard						1
#define TButton						2
#define TLabel						3
#define THighscoreitem				4
#define TTextbox					5
#define TButtonWithFrame			6
#define TCardFrameChosen			7
#define TSiteSelected				8

#define FAILED_LOADING_BACKGROUND	-11

struct Object
{
	int x, y;
	int type;
	int enabled;
	int PairID;

	struct Picture picture;
	struct Button button;
	struct Label label;
};

int IS_NULL(struct Object o);
SDL_Rect *Create_Rect_BO(struct Object *o, int move);
int Save_Objects(struct Object objects[], FILE *f);
struct Object O_New_Button(struct Object O, char Text[], int Button_Type, int Value, int X, int Y);
struct Object O_New_Label(struct Object O, char Text[], int X, int Y);
struct Object O_New_Card(struct Object O, int X, int Y, struct Pair pair, struct Picture pairpicture, struct Picture p);
struct Object O_New_Object(struct Object O, int X, int Y, int Type, char Filename1[], char Filename2[], int ButtonType, int ButtonValue);

#endif