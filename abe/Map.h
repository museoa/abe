#ifndef MAP_H 
#define MAP_H 

#include "Main.h"

// Where to store the map files (no ending /)
#define MAPS_DIR "./maps"

// Tile sizes
#define TILE_W 20
#define TILE_H 20

// The layers of the map
#define LEVEL_BACK 0
#define LEVEL_MAIN 1
#define LEVEL_FORE 2
#define LEVEL_COUNT 3

// Movement directions
#define DIR_UPDATE -2
#define DIR_QUIT -1
#define DIR_NONE 0
#define DIR_UP 1
#define DIR_RIGHT 2
#define DIR_DOWN 3
#define DIR_LEFT 4

// These are the extra tiles to draw on the left and top.
// They should be the tile width and height of the biggest
// images used.
#define EXTRA_X 5
#define EXTRA_Y 5

#define SPEED_INC_X 1
#define SPEED_INC_Y 1

typedef struct _map {
  char *name;
  int w, h;
  int *image_index[LEVEL_COUNT];
  SDL_Surface *level[LEVEL_COUNT];
  SDL_Thread *thread;
  int delay;
  // set this to 1 to kill the movement thread. (by calling: stopMapMoveThread())
  int stopThread;
  // painting callbacks
  void (*beforeDrawToScreen)();
  void (*afterMainLevelDrawn)();
  int (*detectCollision) (int);
  SDL_cond *move_cond;
  SDL_mutex *move_cond_mutex;
  int accelerate; // 1 for accelerated movement, 0 otherwise
} Map;

typedef struct _cursor {
  int dontMove;
  int pos_x, pos_y;
  int pixel_x, pixel_y;
  int speed_x, speed_y;
  int dir;
  int wait;
} Cursor;

Cursor cursor;
Map map;

void drawMap();
void setImage(int level, int index);
void startMapMoveThread();
void stopMapMoveThread();
void signalMapMoveThread();
void initMap(char *name, int w, int h);
void destroyMap();
void saveMap();
int loadMap(int drawMap);
void resetCursor();

// return 1 or 0 if movement in that direction is possible
int moveLeft();
int moveRight();
int moveUp(int checkCollision);
int moveDown();

void lockMap();
void unlockMap();

#endif
