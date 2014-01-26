#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "Pair.h"
#include "Generel_Proc.h"
#include "Picture.h"

#define FailureReadingFile                  -1

int ReadDeck(struct Pair* arraytofill, char* addresstoload)
{
	FILE* file = fopen(addresstoload, "r");
	
	char c[ARRAY_LENGTH];

	if (freadString(file, &c, "<", ARRAY_LENGTH))
		return FailureReadingFile;
	if (freadString(file, &c, ">", ARRAY_LENGTH))
		return FailureReadingFile;
	if (!IsIn(c, "deck"))
		return FailureReadingFile;

	int i = -1;
	while (c[0] != '\0')
	{
		int result;
		if ((result = freadString(file, &c, "<+", ARRAY_LENGTH)) == 0) /*wenn das gleich null ist, ist es ein <*/
		{
			if (freadString(file, &c, ">", ARRAY_LENGTH) == 0)
			{
				if (IsIn(c, "/deck"))
					return i - 1;
				if (!IsIn(c, "pair"))
					return FailureReadingFile;
				else
				if (!IsIn(c, "/pair"))
				{
					i++;
					arraytofill[i + 1].id = -1;
				}
			}
			else
				return FailureReadingFile;
		}
		else if (result == 1)
		{
			if (freadString(file, &c, ":", ARRAY_LENGTH))
				return FailureReadingFile;

			if (IsIn(c, "id"))
			{
				if (freadString(file, &c, "\n", ARRAY_LENGTH))
					return FailureReadingFile;
				arraytofill[i].id = atoi(c);
			}
			else if (IsIn(c, "difficulty"))
			{
				if (freadString(file, &c, "\n", ARRAY_LENGTH))
					return FailureReadingFile;
				arraytofill[i].difficulty = atoi(c);
			}
			else if (IsIn(c, "firstpic"))
			{
				if (freadString(file, &c, "\n", ARRAY_LENGTH))
					return FailureReadingFile;
				strcpy(&arraytofill[i].picture1->filename, &c);
				*arraytofill[i].picture1 = load_picture(*arraytofill[i].picture1, arraytofill[i].picture1->filename);
			}
			else if (IsIn(c, "secondpic"))
			{
				if (freadString(file, &c, "\n", ARRAY_LENGTH))
					return FailureReadingFile;
				strcpy(&arraytofill[i].picture2->filename, &c);
				*arraytofill[i].picture2 = load_picture(*arraytofill[i].picture2, arraytofill[i].picture2->filename);
			}
			else if (IsIn(c, "topic"))
			{
				if (freadString(file, &c, "\n", ARRAY_LENGTH))
					return FailureReadingFile;
				arraytofill[i].topic = atoi(c);
			}
			else if (IsIn(c, "visibility"))
			{
				if (freadString(file, &c, "\n", ARRAY_LENGTH))
					return FailureReadingFile;
				arraytofill[i].visibility = atoi(c);
			}
		}
	}
	
	return i;
}
