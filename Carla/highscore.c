#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <SDL_ttf.h>
#include <Windows.h>

//*Laufvariable z für Züge, die bei jedem Zug um einen erhöht wird, in der Highscoreliste wird dementsprechend das kleinste Ergebnis aufsteigend gespeichert*//

struct shighscore
{
	char Name[20];
	int highscore;
}

	
