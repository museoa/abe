#include "Monster.h"

void defaultMoveMonster(LiveMonster *live) {
  // no-op
}

void moveCrab(LiveMonster *live_monster) {
  live_monster->face++;
  if(live_monster->face >= 2) live_monster->face = 0;
}

/**
   Remember here, images are not yet initialized!
 */
void initMonsters() {
  live_monster_count = 0;
  
  // common properties.
  int i;
  for(i = 0; i < MONSTER_COUNT; i++) {
	monsters[i].image_count = 0;	
	monsters[i].moveMonster = defaultMoveMonster;
  }

  // crab monster
  strcpy(monsters[MONSTER_CRAB].name, "dungenous crab");
  monsters[MONSTER_CRAB].moveMonster = moveCrab;
  monsters[MONSTER_CRAB].type = MONSTER_CRAB;
  monsters[MONSTER_CRAB].start_speed_x = 4;
  monsters[MONSTER_CRAB].start_speed_y = 4;

  // add additional monsters here

  for(i = 0; i < MONSTER_COUNT; i++) {
	fprintf(stderr, "Added monster: %s.\n", monsters[i].name);
  }
  fflush(stderr);
}

void addMonsterImage(int monster_index, int image_index) {
  monsters[monster_index].image_index[monsters[monster_index].image_count++] = image_index;
  fprintf(stderr, "monster image added. monster=%d image_count=%d\n", 
		  monster_index, monsters[monster_index].image_count);
  fflush(stderr);
}

// FIXME: a faster solution is needed. This is O(n^2).
int isMonsterImage(int image_index) {
  int i, t;
  for(i = 0; i < MONSTER_COUNT; i++) {
	for(t = 0; t < monsters[i].image_count; t++) {
	  if(monsters[i].image_index[t] == image_index) return i;
	}
  }
  return -1;
}

void addLiveMonster(int monster_index, int image_index, int x, int y) {
  Monster *m = &monsters[monster_index];
  live_monsters[live_monster_count].pos_x = x;
  live_monsters[live_monster_count].pos_y = y;
  live_monsters[live_monster_count].pixel_x = 0;
  live_monsters[live_monster_count].pixel_y = 0;
  live_monsters[live_monster_count].speed_x = m->start_speed_x;
  live_monsters[live_monster_count].speed_y = m->start_speed_y;
  live_monsters[live_monster_count].dir = DIR_NONE;
  live_monsters[live_monster_count].face = 0;
  live_monsters[live_monster_count].monster = m;
  live_monster_count++;
  fprintf(stderr, "Added live monster! monster=%d x=%d y=%d count=%d\n", 
		  monster_index, x, y, live_monster_count);
  fflush(stderr);
}

void removeLiveMonster(int live_monster_index) {
  /*

  // this crashes... also it should add it back into the map

  int t;
  for(t = live_monster_index; t < live_monster_count; t++) {
	memcpy(&live_monsters[t], &live_monsters[t + 1], sizeof(LiveMonster));
  }
  live_monster_count--;
  */
  fprintf(stderr, "Removed live monster! monster=%d index=%d count=%d\n", 
		  live_monsters[live_monster_index].monster->type, live_monster_index, live_monster_count);
  fflush(stderr);
}

void drawLiveMonsters(SDL_Surface *surface, MapDrawParams *params) {
  SDL_Rect pos;
  SDL_Surface *img;
  int i;
  for(i = 0; i < live_monster_count; i++) {
	live_monsters[i].monster->moveMonster(&live_monsters[i]);

	img = images[live_monsters[i].monster->image_index[live_monsters[i].face]]->image;
	pos.x = (live_monsters[i].pos_x - params->start_x - EXTRA_X) * TILE_W + live_monsters[i].pixel_x;
	pos.y = (live_monsters[i].pos_y - params->start_y - EXTRA_Y) * TILE_H + live_monsters[i].pixel_y;
	pos.w = img->w;
	pos.h = img->h;

	if(!isOnScreen(pos)) {
	  removeLiveMonster(i);
	} else {
	  /*
	  fprintf(stderr, "Drawing live monster! monster=%d index=%d pos: %d,%d screen: %d,%d\n", 
			  live_monsters[i].monster->type, i, 
			  live_monsters[i].pos_x, live_monsters[i].pos_y,
			  pos.x, pos.y);
			  fflush(stderr);
	  */
	  SDL_BlitSurface(img, NULL, surface, &pos);
	}
  }
}
