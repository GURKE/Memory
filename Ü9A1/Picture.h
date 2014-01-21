#ifndef _PICTURE_H_
#define _PICTURE_H_

#pragma warning( disable : 4996 )

#define FILENAME_LENGHT 200

TTF_Font *font;

struct Picture
{
	int ID;
	SDL_Surface *picture;
	char filename[FILENAME_LENGHT];
};

extern struct Picture load_picture(struct Picture p, char Filename[FILENAME_LENGHT]);
extern struct Picture Create_Picture_By_Text(struct Picture p, char Text[FILENAME_LENGHT], int Highlighted);
extern int Save_Picture(FILE *f, struct Picture p);
#endif