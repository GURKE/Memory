#ifndef _GENEREL_PROC_H_
#define _GENEREL_PROC_H_

#pragma warning( disable : 4996 )

#define ARRAY_LENGTH					150
#define FAILED_LOADING_IMAGE			-3

int freadString(FILE *f, char(*Output)[], char seperator); // Returns 0 for ok, -1 for string is too long
char* concat(char *s1, char *s2);
int IsIn(char Text[], char Texttofind[]);

#endif