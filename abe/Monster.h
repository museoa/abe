#ifndef MONSTER_H 
#define MONSTER_H

#include "Main.h"
#include "Map.h"

#define MONSTER_CRAB 0
#define MONSTER_COUNT 1

struct _monster;

// The extra number of tiles around the screen 
// where monsters are still active
#define MONSTER_EXTRA_X 20
#define MONSTER_EXTRA_Y 20

/**
   A monster instance currently on screen.
 */
typedef struct _liveMonster {
  int pos_x;
  int pos_y;
  int pixel_x;
  int pixel_y;
  int speed_x;
  int speed_y;
  int dir;
  int face;
  struct _monster *monster;
} LiveMonster;

/**
   A monster class.
*/
typedef struct _monster {
  int type;
  char name[40];
  int image_count;
  int image_index[256];
  int start_speed_x;
  int start_speed_y;
  int face_mod;
  void (*moveMonster) (LiveMonster *live);
} Monster;

Monster monsters[256];
LiveMonster live_monsters[256];
int live_monster_count;

void initMonsters();
void addMonsterImage(int monster_index, int image_index);
int isMonsterImage(int image_index);

void addLiveMonster(int monster_index, int image_index, int x, int y);
void drawLiveMonsters(SDL_Surface *surface, int start_x, int start_y);

#endif
