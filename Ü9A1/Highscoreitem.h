#ifndef _HIGHSCOREITEM_H_
#define _HIGHSCOREITEM_H_

#define HS_LENGTH 10
#define NAME_LENGTH 20
 
#pragma warning( disable : 4996 )

struct Highscoreitem
{
	int ranking;
	char name[NAME_LENGTH];
	int moves;
};

#endif						// !_HIGHSCOREITEM_H_
