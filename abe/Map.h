#ifndef MAP_H 
#define MAP_H 

#include "Main.h"

// Where to store the map files (no ending /)
#define MAPS_DIR "maps"

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

// if this is 1, it uses a transfer surface to scroll
// else it uses memmove() unless screen is a HWSURFACE
// in which case this is ignored and a transfer surface
// is used.
#define FORCE_SDL_SCROLL 0

// an empty map position
#define EMPTY_MAP 0xffff

typedef struct _map {
  char *name;
  Uint16 w, h;
  Uint16 *image_index[LEVEL_COUNT];
  SDL_Surface *level[LEVEL_COUNT];
  SDL_Surface *transfer;
  int delay;
  // painting callbacks
  void (*beforeDrawToScreen)();
  void (*afterMainLevelDrawn)();
  int (*detectCollision) (int);
  int (*detectLadder) ();
  void (*handleMapEvent) (SDL_Event*);
  int accelerate; // 1 for accelerated movement, 0 otherwise(default)
  int gravity; // 1 for gravity, 0 otherwise(default)
  int monsters; // 1 for active monsters, 0 otherwise(default)
  int redraw; // set to 1 to cause a full map repaint
} Map;

// TODO: reuse Position inside Cursor.
typedef struct _cursor {
  int pos_x, pos_y;
  int pixel_x, pixel_y;
  int speed_x, speed_y;
  int dir;
  int wait;
  int jump;
  int gravity;
  int stepup;
} Cursor;

Cursor cursor;
Map map;

// the main map loop routine
void moveMap();

void drawMap();
void setImage(int level, int index);
void setImageNoCheck(int level, int x, int y, int image_index);
int initMap(char *name, int w, int h);
void resetMap();
void destroyMap();
void resetCursor();
void repositionCursor(int tile_x, int tile_y);

// return 1 or 0 if movement in that direction is possible
int moveLeft(int checkCollision);
int moveRight(int checkCollision);
int moveUp(int checkCollision);
int moveDown();

void lockMap();
void unlockMap();

void startJump();
int containsType(Position *p, int type);
// like contains type, but returns the position of the object in ret.
int containsTypeWhere(Position *p, Position *ret, int type);
int onSolidGround(Position *p);

#endif
