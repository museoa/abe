#ifndef MONSTER_H 
#define MONSTER_H

#include "Main.h"
#include "Map.h"

#define MONSTER_CRAB 0
#define MONSTER_SMASHER 1
#define MONSTER_DEMON 2
#define MONSTER_SMASHER2 3
#define MONSTER_PLATFORM 4
#define MONSTER_PLATFORM2 5
#define MONSTER_SPIDER 6
#define MONSTER_COUNT 7

struct _monster;

// if the intersection of tom and monster is bigger than this number
// it is considered a hit. A MONSTER_COLLISION_FUZZ of 0 is the least
// tolerant and a TILE_W is the most.
#define MONSTER_COLLISION_FUZZ 4

// The extra number of tiles around the screen 
// ,.where monsters are still active
#define MONSTER_EXTRA_X 80
#define MONSTER_EXTRA_Y 60

// The max amount of speed change
#define MAX_RANDOM_SPEED 6.0

Monster monsters[256];
LiveMonster live_monsters[256];
int live_monster_count;
int move_monsters;

void initMonsters();
void resetMonsters();
void addMonsterImage(int monster_index, int image_index);
int isMonsterImage(int image_index);

void addLiveMonster(int monster_index, int image_index, int x, int y);
void removeAllLiveMonsters();
void drawLiveMonsters(SDL_Surface *surface, int start_x, int start_y);

/**
   Return live monster if there's a one at position pos,
   NULL otherwise.
 */
LiveMonster *detectMonster(Position *pos);

#endif
