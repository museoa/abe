#ifndef GAME_H 
#define GAME_H 

#include "Main.h"

#define FACE_STILL 1
#define FACE_COUNT 3
#define TOM_W 4
#define TOM_H 4

#define FACE_STEP 2

#define GOD_MODE 1

#define BALLOON_RIDE_INTERVAL 500

#define SPRING_JUMP 30

#define SAVEGAME_DIR "savegame"

#define MAX_HEALTH 100

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
} Game;
extern Game game;

void initGame();
void runMap();
void gameMainLoop(SDL_Event *event);

#endif
