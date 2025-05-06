#ifndef COULEURS
#define COULEURS

#include <stdio.h>

#define clrscr() printf("\033[H\033[2J")

#define couleur(param) printf("\033[%sm",param)

#define BLANC "37"
#define FOND_NOIR "40"

#define RESET "0"        

#endif
