#ifndef GAME_H 
#define GAME_H 

#include "Main.h"

#define FACE_STILL 1
#define FACE_COUNT 3
#define CLIMB_FACE_COUNT 3
#define CLIMB_FACE_MIN 10
#define TOM_W 4
#define TOM_H 4

#define FACE_STEP 2

// really: development mode
#define GOD_MODE 1

#define BALLOON_RIDE_INTERVAL 500

#define SPRING_JUMP 30

#define SAVEGAME_DIR "savegame"

#define MAX_HEALTH 100

// don't hold more than these... mainly b/c the gui can't handle it
#define MAX_BALLOONS 7
#define MAX_KEYS 7

// increment this to avoid saved-game conflict
// abe0.1, 0.2, 0.3 = 1 (actually no version number is saved)
// abe0.4 = 2
#define GAME_VERSION 2

typedef struct _game {
  int face;
  int dir;
  int player_start_x, player_start_y;
  int lives;
  int score;
  int draw_player;
  int keys;
  int balloons;
  int balloonTimer;
  int god_mode;
  int lastSavePosX;
  int lastSavePosY;
  int health;
  int difficoulty;
  int dir_changed;
  int in_water;
  int tick;
} Game;
extern Game game;

void initGame();
void runMap();
void gameMainLoop(SDL_Event *event);

#endif
