#ifndef GAME_H 
#define GAME_H 

#include "Main.h"

#define FACE_STILL 1
#define FACE_COUNT 3
#define GAME_DIR_LEFT 0
#define GAME_DIR_RIGHT 1
#define TOM_W 4
#define TOM_H 4

#define FACE_STEP 2

typedef struct _game {
  int face;
  int dir;
} Game;
Game game;

void initGame();
void runMap(char *name, int w, int h);
void gameMainLoop(SDL_Event *event);

#endif
