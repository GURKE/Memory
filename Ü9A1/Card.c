#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

#include "cards.h"
#inlcude "General_Proc.h"

#define FailureReadingFile                  -1

int ReadDeck(pair* arraytofill, char* addresstoload)
{
	FILE* file = fopen(*adresstoload,"r");

    struct pair availablecardsarray[ARRAY_LENGTH];
	char c[ARRAY_LENGTH];

    if (freadString(file,&c,'<',ARRAY_LENGTH))
        return FailureReadingFile;
    if (freadString(file,&c,'>',ARRAY_LENGTH))
        return FailureReadingFile;
    if (c != "deck")
        return FailureReadingFile;

    int i = -1;
	while (c[0] != '\0')
	{
	    int result;
        if ((result = freadString(file,&c,"<+",ARRAY_LENGTH)) == 0) /*wenn das gleich null ist, ist es ein <*/
        {
            if (freadString(file,&c,">",ARRAY_LENGTH) == 0)
            {
                if (c != "pair")
                    return FailureReadingFile;
                else
                    i++;
            }
            else
                return FailureReadingFile;
        }
        else if (result == 1)
        {
            if (freadString(file,&c,':',ARRAY_LENGTH))
                return FailureReadingFile;

            switch(c)
            {
                case "id"
                {
                    if (freadString(file,&c,'\n',ARRAY_LENGTH))
                        return FailureReadingFile;
                    availablecardsarray[i].id = atoi(c);
                    break;
                }
                case "difficulty"
                {
                    if (freadString(file,&c,'\n',ARRAY_LENGTH))
                        return FailureReadingFile;
                    availablecardsarray[i].difficulty = atoi(c);
                    break;
                }
                case "location"
                {
                    if (freadString(file,&c,'\n',ARRAY_LENGTH))
                        return FailureReadingFile;
                    strcpy(&*availablecardsarray[i].location*,&*atoi(c)*);
                    break;
                }
                case "topic"
                {
                    if (freadString(file,&c,'\n',ARRAY_LENGTH))
                        return FailureReadingFile;
                    availablecardsarray[i].topic = atoi(c);
                    break;
                }
                case "visibility"
                {
                    if (freadString(file,&c,'\n',ARRAY_LENGTH))
                        return FailureReadingFile;
                    availablecardsarray[i].visibility = atoi(c);
                    break;
                }
                defualt:
                {
                    break;
                }
            }
        }
	}

    return availablecardsarray [i];
}
