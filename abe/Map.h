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

// the speed increments in accelerated mode
#define SPEED_INC_X 2
#define SPEED_INC_Y 2

// the minimum (starting) speed in accelerated mode
#define START_SPEED_X 6
#define START_SPEED_Y 6

// how many units is in a jump?
#define JUMP_LENGTH 7
#define JUMP_SPEED 14

typedef struct _map {
  char *name;
  int w, h;
  int *image_index[LEVEL_COUNT];
  SDL_Surface *level[LEVEL_COUNT];
  int delay;
  // set this to 1 to kill the movement thread. (by calling: stopMapMoveThread())
  int stopThread;
  // painting callbacks
  void (*beforeDrawToScreen)();
  void (*afterMainLevelDrawn)();
  int (*detectCollision) (int);
  void (*handleMapEvent) (SDL_Event*);
  int accelerate; // 1 for accelerated movement, 0 otherwise
  int gravity; // 1 for gravity, 0 otherwise(default)
} Map;

typedef struct _cursor {
  int pos_x, pos_y;
  int pixel_x, pixel_y;
  int speed_x, speed_y;
  int dir;
  int wait;
  int jump;
} Cursor;

Cursor cursor;
Map map;

void drawMap();
void setImage(int level, int index);
void initMap(char *name, int w, int h);
void destroyMap();
void saveMap();
int loadMap(int drawMap);
void resetCursor();

// return 1 or 0 if movement in that direction is possible
int moveLeft(int checkCollision, int scroll);
int moveRight(int checkCollision, int scroll);
int moveUp(int checkCollision, int scroll);
int moveDown();

void lockMap();
void unlockMap();

/** Remove unnecesary -1s. For example a 4 tile wide stone becomes a 1 int number.
	return new number of elements in new_size. (so num of bytes=new_size * sizeof(int)).
	caller must free returned pointer.
	call this method before calling Utils.compress(). This prepares the map
	for better compression by understanding the its structure. This doesn't 
	compress the map that much, but combined with Utils.compress() map files
	can go from 12M to 14K!
*/
int *compressMap(size_t *new_size);
void decompressMap();

void startJump();

#endif
