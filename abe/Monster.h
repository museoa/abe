#ifndef MONSTER_H 
#define MONSTER_H

#include "Main.h"
#include "Map.h"

#define MONSTER_CRAB 0
#define MONSTER_SMASHER 1
#define MONSTER_DEMON 2
#define MONSTER_SMASHER2 3
#define MONSTER_PLATFORM 4
#define MONSTER_COUNT 5

struct _monster;

// The extra number of tiles around the screen 
// ,.where monsters are still active
#define MONSTER_EXTRA_X 40
#define MONSTER_EXTRA_Y 30

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
void drawLiveMonsters(SDL_Surface *surface, int start_x, int start_y);

/**
   Return live monster if there's a one at position pos,
   NULL otherwise.
 */
LiveMonster *detectMonster(Position *pos);

#endif
