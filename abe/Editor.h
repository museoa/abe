#ifndef EDITOR_H 
#define EDITOR_H 

#include <sys/types.h>
#include <dirent.h>
#include "Main.h"

// Tile sizes
#define TILE_W 20
#define TILE_H 20
#define MAPS_DIR "./maps"

// Movement directions
#define DIR_QUIT -1
#define DIR_NONE 0
#define DIR_UP 1
#define DIR_RIGHT 2
#define DIR_DOWN 3
#define DIR_LEFT 4

// The layers of the map
#define LEVEL_BACK2 0
#define LEVEL_BACK1 1
#define LEVEL_MAIN 2
#define LEVEL_FORE 3
#define LEVEL_COUNT 4

// These are the extra tiles to draw on the left and top.
// They should be the tile width and height of the biggest
// images used.
#define EXTRA_X 5
#define EXTRA_Y 5

#define EDIT_PANEL_HEIGHT 120

typedef struct _editpanel {
  SDL_Surface *image;
  int level;
  int image_index;
} EditPanel;

typedef struct _map {
  char *name;
  int w, h;
  int *image_index[LEVEL_COUNT];
  SDL_Surface *level[LEVEL_COUNT];
} Map;

typedef struct _cursor {
  int pos_x, pos_y;
  int dir;
  int wait;
} Cursor;

Cursor cursor;
Map map;
EditPanel edit_panel;

void initEditor();
void editMap(char *name, int w, int h);
void saveMap();
void loadMap(char *name);
void drawMap();
int moveMap(void *); // thread function
void editorMainLoop(SDL_Event *event);
void setImage(int level, int index);
void drawEditPanel();

#endif
