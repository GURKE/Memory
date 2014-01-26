#ifndef _CARD_H_
#define _CARD_H_

#include "Picture.h"

#pragma warning( disable : 4996 )
#define FAILED_LOADING_IMAGE			-3

struct Card
{
	Uint16 difficulty; // 0 = easy ... 3 = hard
	Uint16 type; // Pair-ID
	int visible; // 0 = background of the card, 1 = foreground
	struct Picture *picture;
};

int init_cards_old(struct Card (*cards)[], char FileName[]); // returns the amount of cards
int Save_Card(FILE *f, struct Card c);

#endif