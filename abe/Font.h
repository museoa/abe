#ifndef FONT_H 
#define FONT_H 

#include "Main.h"

#define FONT_WIDTH 22
#define FONT_HEIGHT 40
#define FONT_SPACE -3

SDL_Surface *fonts;

void initFonts(SDL_Surface *image);
void drawLetter(SDL_Surface *surface, int x, int y, char letter);
void drawString(SDL_Surface *surface, int x, int y, char *str);

#endif
