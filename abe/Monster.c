#include "Monster.h"

int getLiveMonsterFace(LiveMonster *live);

SDL_Rect extended_screen_rect;

void defaultMoveMonster(LiveMonster *live) {
  // no-op
}

void defaultDrawMonster(SDL_Rect *pos, LiveMonster *live, SDL_Surface *surface, SDL_Surface *img) {
  SDL_BlitSurface(img, NULL, surface, pos);
}

void initMonsterPos(Position *pos, LiveMonster *live) {
  pos->pos_x = live->pos_x;
  pos->pos_y = live->pos_y;
  pos->pixel_x = live->pixel_x;
  pos->pixel_y = live->pixel_y;
  pos->w = images[live->monster->image_index[0]]->image->w / TILE_W;
  pos->h = images[live->monster->image_index[0]]->image->h / TILE_H;
}

int stepMonsterLeft(LiveMonster *live) {
  Position pos;
  int fail = 0;
  LiveMonster old;
  memcpy(&old, live, sizeof(LiveMonster));
  live->pixel_x -= live->speed_x;
  if(live->pixel_x < 0) {
	live->pos_x--;
	live->pixel_x = TILE_W + live->pixel_x;
	if(live->pos_x < 0) {
	  fail = 1;
	}
  }
  // collision detection
  if(!fail) {
	initMonsterPos(&pos, live);
	if(containsType(&pos, TYPE_WALL | TYPE_DOOR) || !onSolidGround(&pos)) fail = 1;
  }
  if(fail) {
	memcpy(live, &old, sizeof(LiveMonster));
	return 0;
  }
  return 1;
}

int stepMonsterUp(LiveMonster *live) {
  Position pos;
  int fail = 0;
  LiveMonster old;
  memcpy(&old, live, sizeof(LiveMonster));
  live->pixel_y -= live->speed_y;
  if(live->pixel_y < 0) {
	live->pos_y--;
	live->pixel_y = TILE_H + live->pixel_y;
	if(live->pos_y < 0) {
	  fail = 1;
	}
  }
  // collision detection
  if(!fail) {
	initMonsterPos(&pos, live);
	if(containsType(&pos, TYPE_WALL | TYPE_DOOR)) fail = 1;
  }
  if(fail) {
	memcpy(live, &old, sizeof(LiveMonster));
	return 0;
  }
  return 1;
}

int stepMonsterRight(LiveMonster *live) {
  Position pos;
  int fail = 0;
  LiveMonster old;
  memcpy(&old, live, sizeof(LiveMonster));
  live->pixel_x += live->speed_x;
  if(live->pixel_x >= TILE_W) {
	live->pos_x++;
	live->pixel_x = live->pixel_x - TILE_W;
	if(live->pos_x >= map.w) {
	  fail = 1;
	}
  }
  // collision detection
  if(!fail) {
	initMonsterPos(&pos, live);
	if(containsType(&pos, TYPE_WALL | TYPE_DOOR) || !onSolidGround(&pos)) fail = 1;
  }
  if(fail) {
	memcpy(live, &old, sizeof(LiveMonster));
	return 0;
  }
  return 1;
}

int stepMonsterDown(LiveMonster *live) {
  Position pos;
  int fail = 0;
  LiveMonster old;
  memcpy(&old, live, sizeof(LiveMonster));
  live->pixel_y += live->speed_y;
  if(live->pixel_y >= TILE_H) {
	live->pos_y++;
	live->pixel_y = live->pixel_y - TILE_H;
	if(live->pos_y >= map.h) {
	  fail = 1;
	}
  }
  // collision detection
  if(!fail) {
	initMonsterPos(&pos, live);
	if(containsType(&pos, TYPE_WALL | TYPE_DOOR)) fail = 1;
  }
  if(fail) {
	memcpy(live, &old, sizeof(LiveMonster));
	return 0;
  }
  return 1;
}

void moveCrab(LiveMonster *live_monster) {
  // increment the face to display
  live_monster->face++;
  if(live_monster->face >= 
	 live_monster->monster->image_count * live_monster->monster->face_mod) 
	live_monster->face = 0;

  // move sideways until you hit a wall or an edge
  if(live_monster->dir == DIR_LEFT) {
	if(!stepMonsterLeft(live_monster)) {
	  live_monster->dir = DIR_RIGHT;
	}
  } else {
	if(!stepMonsterRight(live_monster)) {
	  live_monster->dir = DIR_LEFT;
	}
  }
}

void moveSmasher(LiveMonster *live_monster) {
  // move up and down until you hit an edge
  if(live_monster->dir == DIR_DOWN) {
	if(!stepMonsterDown(live_monster)) {
	  live_monster->dir = DIR_UP;
	  live_monster->speed_y = 2;
	}
  } else {
	if(!stepMonsterUp(live_monster)) {
	  live_monster->dir = DIR_DOWN;
	  live_monster->speed_y = 8;
	}
  }
}

void drawSmasher(SDL_Rect *pos, LiveMonster *live, SDL_Surface *surface, SDL_Surface *img) {
  SDL_Rect p, q;
  Position position;

  p.x = pos->x;
  p.y = (pos->y / TILE_H) * TILE_H - TILE_H;
  p.w = pos->w;
  p.h = pos->h;

  position.pos_x = live->pos_x;
  position.pos_y = live->pos_y - 1;
  position.pixel_x = 0;
  position.pixel_y = 0;
  position.w = p.w / TILE_W;
  position.h = p.h / TILE_H;

  while(position.pos_y >= 0 && 
		!containsType(&position, TYPE_WALL | TYPE_DOOR)) {
	SDL_BlitSurface(images[img_smash2]->image, NULL, surface, &p);
	p.y -= TILE_H;
	position.pos_y--;	
  }

  if(live->pixel_y) {
	p.y = pos->y - live->pixel_y;
	q.x = q.y = 0;
	q.w = p.w;
	q.h = live->pixel_y;
	SDL_BlitSurface(images[img_smash2]->image, &q, surface, &p);
  }

  SDL_BlitSurface(img, NULL, surface, pos);
}

/**
   Remember here, images are not yet initialized!
 */
void initMonsters() {
  int i;

  move_monsters = 1;
  live_monster_count = 0;

  // init the screen rectangle.
  extended_screen_rect.x = -MONSTER_EXTRA_X * TILE_W;
  extended_screen_rect.y = -MONSTER_EXTRA_Y * TILE_H;
  extended_screen_rect.w = screen->w + 2 * MONSTER_EXTRA_X * TILE_W;
  extended_screen_rect.h = screen->h + 2 * MONSTER_EXTRA_Y * TILE_H;
  
  // common properties.
  for(i = 0; i < MONSTER_COUNT; i++) {
	monsters[i].image_count = 0;	
	monsters[i].moveMonster = defaultMoveMonster;
	monsters[i].drawMonster = defaultDrawMonster;
	monsters[i].face_mod = 1;
  }

  // crab monster
  strcpy(monsters[MONSTER_CRAB].name, "dungenous crab");
  monsters[MONSTER_CRAB].moveMonster = moveCrab;
  monsters[MONSTER_CRAB].type = MONSTER_CRAB;
  monsters[MONSTER_CRAB].start_speed_x = 4;
  monsters[MONSTER_CRAB].start_speed_y = 4;
  // animation 2x slower
  monsters[MONSTER_CRAB].face_mod = 2;

  // smasher monster
  strcpy(monsters[MONSTER_SMASHER].name, "smasher");
  monsters[MONSTER_SMASHER].moveMonster = moveSmasher;
  monsters[MONSTER_SMASHER].drawMonster = drawSmasher;
  monsters[MONSTER_SMASHER].type = MONSTER_SMASHER;
  monsters[MONSTER_SMASHER].start_speed_x = 8;
  monsters[MONSTER_SMASHER].start_speed_y = 8;

  // add additional monsters here

  for(i = 0; i < MONSTER_COUNT; i++) {
	fprintf(stderr, "Added monster: %s.\n", monsters[i].name);
  }
  fflush(stderr);
}

void resetMonsters() {
  live_monster_count = 0;
}

void addMonsterImage(int monster_index, int image_index) {
  monsters[monster_index].image_index[monsters[monster_index].image_count++] = image_index;
  fprintf(stderr, "monster image added. monster=%d image_count=%d\n", 
		  monster_index, monsters[monster_index].image_count);
  fflush(stderr);
}

int isMonsterImage(int image_index) {
  // new fast way of doing this.
  if(image_index == EMPTY_MAP) return -1;
  return images[image_index]->monster_index;  
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
  int t;
  LiveMonster *p;

  // add it back to the map
  p = &live_monsters[live_monster_index];
  setImageNoCheck(LEVEL_MAIN, 
				  p->pos_x, p->pos_y, 
				  p->monster->image_index[getLiveMonsterFace(p)]);

  // remove it from memory
  for(t = live_monster_index; t < live_monster_count - 1; t++) {
	memcpy(&live_monsters[t], &live_monsters[t + 1], sizeof(LiveMonster));
  }
  live_monster_count--;

  fprintf(stderr, "Removed live monster! monster=%d index=%d count=%d\n", 
		  live_monsters[live_monster_index].monster->type, live_monster_index, live_monster_count);
  fflush(stderr);
}

/**
   Here rect is in pixels where 0, 0 is the screen's left top corner.
   Returns 0 for false and non-0 for true.
 */
int isOnScreen(SDL_Rect rect) {
  return intersects(&rect, &extended_screen_rect);
}

/**
   Draw all currently tracked creatures.
   start_x, start_y are the offset of the screen's top left edge in pixels.
 */
void drawLiveMonsters(SDL_Surface *surface, int start_x, int start_y) {
  SDL_Rect pos;
  SDL_Surface *img;
  int face;
  int i;

  for(i = 0; i < live_monster_count; i++) {
	if(move_monsters)
	  live_monsters[i].monster->moveMonster(&live_monsters[i]);
	
	img = images[live_monsters[i].monster->image_index[getLiveMonsterFace(&live_monsters[i])]]->image;
	pos.x = live_monsters[i].pos_x * TILE_W - start_x + live_monsters[i].pixel_x;
	pos.y = live_monsters[i].pos_y * TILE_H - start_y + live_monsters[i].pixel_y;
	pos.w = img->w;
	pos.h = img->h;

	if(!isOnScreen(pos)) {
	  removeLiveMonster(i);
	} else {
	  live_monsters[i].monster->drawMonster(&pos, &live_monsters[i], surface, img);
	}
  }
}

int getLiveMonsterFace(LiveMonster *live) {
  return live->face / live->monster->face_mod;
}

/**
   Return 1 if there's a live monster at position pos,
   0 otherwise.
 */
int detectMonster(Position *pos) {
  int i;
  SDL_Rect monster, check;
  SDL_Surface *img;

  check.x = pos->pos_x;
  check.y = pos->pos_y;
  check.w = pos->w + (pos->pixel_x > 0 ? 1 : 0);
  check.h = pos->h + (pos->pixel_y > 0 ? 1 : 0);
  for(i = 0; i < live_monster_count; i++) {
	img = images[live_monsters[i].monster->image_index[getLiveMonsterFace(&live_monsters[i])]]->image;
	monster.x = live_monsters[i].pos_x;
	monster.y = live_monsters[i].pos_y;
	monster.w = (img->w / TILE_W) + (live_monsters[i].pixel_x > 0 ? 1 : 0);
	monster.h = (img->h / TILE_H) + (live_monsters[i].pixel_y > 0 ? 1 : 0);
	if(intersects(&check, &monster)) return 1;
  }
  return 0;
}
