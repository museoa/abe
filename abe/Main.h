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

#define STATE_SPLASH_SCREEN 0
#define STATE_MAIN_LOOP 1
#define STATE_EDIT_LOOP 2
#define STATE_MENU 3
#define STATE_GAME_LOOP 4

#endif
