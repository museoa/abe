#ifndef MAIN_H 
#define MAIN_H

#include <stdlib.h>
#include <string.h>
#include "SDL.h"
#include "SDL_thread.h"
#include "Image.h"
#include "Font.h"
#include "Util.h"
#include "Menu.h"
#include "Map.h"

SDL_Surface *screen;
int state;

void startEditor();
void startGame();

#endif
