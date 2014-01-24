#ifndef _CARD_H_
#define _CARD_H_

#include "Picture.h"

#pragma warning( disable : 4996 )
#define FAILED_LOADING_IMAGE			-3
#define ARRAY_LENGTH					150

struct Card
{
	Uint16 difficulty; // 0 = easy ... 3 = hard
	Uint16 type; // Pair-ID
	int visible; // 0 = background of the card, 1 = foreground
	struct Picture *picture;
};

int init_cards(struct Card (*cards)[], char FileName[]); // returns the amount of cards
int Save_Card(FILE *f, struct Card c);
int freadString(FILE *f, char(*Output)[], char seperator, int MaxLength); // Returns 0 for ok, -1 for string is too long

#endif