#include "Monster.h"

Monster monsters[256];
LiveMonster live_monsters[256];
int live_monster_count;
int move_monsters;

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

int stepMonsterLeft(LiveMonster *live, int float_ok) {
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
	if(containsType(&pos, TYPE_WALL | TYPE_DOOR) || !(float_ok || onSolidGround(&pos))) fail = 1;
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

int stepMonsterRight(LiveMonster *live, int float_ok) {
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
	if(containsType(&pos, TYPE_WALL | TYPE_DOOR) || !(float_ok || onSolidGround(&pos))) fail = 1;
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
	if(live->pos_y >= map.h) fail = 1;
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

void moveDemon(LiveMonster *live_monster) {
  int j;

  // move sideways until you hit a wall or an edge
  j=1+(int) (10.0*rand()/(RAND_MAX));
  if(j > 7) {
	// increment the face to display
	live_monster->face++;
	if(live_monster->face >= 
	   live_monster->monster->image_count * live_monster->monster->face_mod) 
	  live_monster->face = 0;	

	if(live_monster->dir == DIR_LEFT) {
	  if(!stepMonsterLeft(live_monster, 0)) {
		live_monster->dir = DIR_RIGHT;
	  }
	} else {
	  if(!stepMonsterRight(live_monster, 0)) {
		live_monster->dir = DIR_LEFT;
	  }
	}
  }
}

void movePlatform(LiveMonster *live_monster) {
  // increment the face to display
  live_monster->face++;
  if(live_monster->face >= 
	 live_monster->monster->image_count * live_monster->monster->face_mod) 
	live_monster->face = 0;

  // move sideways until you hit a wall or an edge
  if(live_monster->dir == DIR_LEFT) {
	if(!stepMonsterLeft(live_monster, 1)) {
	  live_monster->dir = DIR_RIGHT;
	}
  } else {
	if(!stepMonsterRight(live_monster, 1)) {
	  live_monster->dir = DIR_LEFT;
	}
  }
}

void movePlatform2(LiveMonster *live_monster) {
  // increment the face to display
  live_monster->face++;
  if(live_monster->face >= 
	 live_monster->monster->image_count * live_monster->monster->face_mod) 
	live_monster->face = 0;

  // move up and down
  if(live_monster->dir == DIR_DOWN) {
	if(!stepMonsterDown(live_monster)) {
	  live_monster->dir = DIR_UP;
	}
  } else {
	if(!stepMonsterUp(live_monster)) {
	  live_monster->dir = DIR_DOWN;
	}
  }
}

void moveCrab(LiveMonster *live_monster) {
  // increment the face to display
  live_monster->face++;
  if(live_monster->face >= 
	 live_monster->monster->image_count * live_monster->monster->face_mod) 
	live_monster->face = 0;

  // move sideways until you hit a wall or an edge
  if(live_monster->dir == DIR_LEFT) {
	if(!stepMonsterLeft(live_monster, 0)) {
	  live_monster->dir = DIR_RIGHT;
	}
  } else {
	if(!stepMonsterRight(live_monster, 0)) {
	  live_monster->dir = DIR_LEFT;
	}
  }
}

void moveArrow(LiveMonster *live_monster) {
  // increment the face to display
  int old = getLiveMonsterFace(live_monster);
  int face;
  int n = (int) (100.0*rand()/(RAND_MAX+1.0));
  if(n > 0) return;

  live_monster->face++;
  if(live_monster->face >= 
	 live_monster->monster->image_count * live_monster->monster->face_mod) {
	live_monster->face = 0;
  }
  face = getLiveMonsterFace(live_monster);
  if(old < face) {
	live_monster->pos_y--;
  } else if(old > face) {
	live_monster->pos_y++;
  }
}

void moveTorch(LiveMonster *live_monster) {
  // increment the face to display
  live_monster->face++;
  if(live_monster->face >= 
	 live_monster->monster->image_count * live_monster->monster->face_mod) 
	live_monster->face = 0;
}

void moveBear(LiveMonster *live_monster) {
  // increment the face to display
  int n = live_monster->monster->image_count / 2;
  live_monster->face++;
  // move sideways until you hit a wall or an edge
  if(live_monster->dir == DIR_LEFT) {
	if(live_monster->face >= n * live_monster->monster->face_mod) 
	  live_monster->face = 0;
	if(!stepMonsterLeft(live_monster, 0)) {
	  live_monster->dir = DIR_RIGHT;
	  live_monster->face = 0;
	}
  } else {
	if(live_monster->face >= live_monster->monster->image_count * live_monster->monster->face_mod) 
	  live_monster->face = n * live_monster->monster->face_mod;
	if(!stepMonsterRight(live_monster, 0)) {
	  live_monster->dir = DIR_LEFT;
	  live_monster->face = n * live_monster->monster->face_mod;
	}
  }
}

void moveFire(LiveMonster *live_monster) {
  // move up and down until you hit an edge
  if(live_monster->dir == DIR_DOWN) {
	if(!stepMonsterDown(live_monster)) {
	  live_monster->dir = DIR_UP;
	  live_monster->speed_y = live_monster->monster->start_speed_y + ((int)((MAX_RANDOM_SPEED / 2) * rand()/(RAND_MAX)));
	  if(live_monster->speed_y <= 0) live_monster->speed_y = 1;
	}
  } else {
	if(!stepMonsterUp(live_monster)) {
	  live_monster->dir = DIR_DOWN;
	  live_monster->speed_y = live_monster->monster->start_speed_y + ((int)((MAX_RANDOM_SPEED / 2) * rand()/(RAND_MAX)));
	  if(live_monster->speed_y <= 0) live_monster->speed_y = 1;
	}
  }
}

void drawFire(SDL_Rect *pos, LiveMonster *live, SDL_Surface *surface, SDL_Surface *img) {
  int y = 0;
  SDL_Rect p, q;
  Position position;
  int index;

  p.x = pos->x;
  //  p.y = (pos->y / TILE_H) * TILE_H - TILE_H;
  y = p.y = pos->y;
  p.w = pos->w;
  p.h = pos->h;

  position.pos_x = live->pos_x;
  position.pos_y = live->pos_y;
  position.pixel_x = live->pixel_x;
  position.pixel_y = live->pixel_y;
  position.w = p.w / TILE_W;
  position.h = p.h / TILE_H;

  while(position.pos_y < map.h && 
		!containsType(&position, TYPE_WALL | TYPE_DOOR)) {
	index = (int) ((double)(live->monster->image_count) * rand() / RAND_MAX);
	SDL_BlitSurface(images[live->monster->image_index[index]]->image, NULL, surface, &p);
	//	y = p.y;
	//	if(!y) break;
	p.x = pos->x;
	y += TILE_H;
	p.y = y;
	p.w = pos->w;
	p.h = pos->h;
	position.pos_y++;	
  }

  // draw the last one
  if(position.pos_y > live->pos_y && live->pixel_y) {
	index = (int) ((double)(live->monster->image_count) * rand() / RAND_MAX);
	q.x = 0;
	q.y = 0;
	q.w = images[live->monster->image_index[index]]->image->w;
	q.h = images[live->monster->image_index[index]]->image->h - live->pixel_y;
	SDL_BlitSurface(images[live->monster->image_index[index]]->image, &q, surface, &p);	
  }
}

void moveSmasher(LiveMonster *live_monster) {
  // move up and down until you hit an edge
  if(live_monster->dir == DIR_DOWN) {
	if(!stepMonsterDown(live_monster)) {
	  live_monster->dir = DIR_UP;
	  live_monster->speed_y = live_monster->monster->start_speed_y / 2 + ((int)((MAX_RANDOM_SPEED / 2) * rand()/(RAND_MAX)));
	  if(live_monster->speed_y <= 0) live_monster->speed_y = 1;
	}
  } else {
	if(!stepMonsterUp(live_monster)) {
	  live_monster->dir = DIR_DOWN;
	  live_monster->speed_y = live_monster->monster->start_speed_y + ((int)((MAX_RANDOM_SPEED / 2) * rand()/(RAND_MAX)));
	  if(live_monster->speed_y <= 0) live_monster->speed_y = 1;
	}
  }
}

void drawSmasher(SDL_Rect *pos, LiveMonster *live, SDL_Surface *surface, SDL_Surface *img) {
  int y = 0;
  SDL_Rect p, q;
  Position position;
  int first_image, first, second;

  first_image = live->monster->image_index[0];
  first = (first_image == img_smash || first_image == img_smash2 ? img_smash : 
		   (first_image == img_smash3 || first_image == img_smash4 ? img_smash3 :
			img_spider));
  second = (first == img_smash ? img_smash2 : 
		   (first == img_smash3 ? img_smash4 :
			img_spider2));

  p.x = pos->x;
  //  p.y = (pos->y / TILE_H) * TILE_H - TILE_H;
  p.y = pos->y - TILE_H;
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
	SDL_BlitSurface(images[second]->image, NULL, surface, &p);
	// HACK part 1: if p->y is reset to 0 the image was cropped.
	y = p.y;
	if(!y) break;
	p.x = pos->x;
	p.y -= TILE_H;
	p.w = pos->w;
	p.h = pos->h;
	position.pos_y--;	
  }
  // draw the top one.
  // HACK part 1: if y is 0 the image was cropped, don't draw
  if(y && live->pixel_y) {
	p.x = pos->x;
	p.y += TILE_H;
	p.y -= live->pixel_y;
	p.w = pos->w;
	p.h = pos->h;
	
	q.x = 0;
	q.y = TILE_H - live->pixel_y;
	q.w = p.w;
	q.h = images[second]->image->h - q.y;
	SDL_BlitSurface(images[second]->image, &q, surface, &p);
  }

  SDL_BlitSurface(images[first]->image, NULL, surface, pos);
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
	monsters[i].type = i;
	monsters[i].harmless = 0;
	monsters[i].random_speed = 1;
  }

  // crab monster
  strcpy(monsters[MONSTER_CRAB].name, "dungenous crab");
  monsters[MONSTER_CRAB].moveMonster = moveCrab;
  monsters[MONSTER_CRAB].start_speed_x = 2;
  monsters[MONSTER_CRAB].start_speed_y = 2;
  // animation 2x slower
  monsters[MONSTER_CRAB].face_mod = 2;
  monsters[MONSTER_CRAB].random_speed = 0;

  // smasher monster
  strcpy(monsters[MONSTER_SMASHER].name, "smasher");
  monsters[MONSTER_SMASHER].moveMonster = moveSmasher;
  monsters[MONSTER_SMASHER].drawMonster = drawSmasher;
  monsters[MONSTER_SMASHER].start_speed_x = 4;
  monsters[MONSTER_SMASHER].start_speed_y = 4;

  // purple smasher
  strcpy(monsters[MONSTER_SMASHER2].name, "smasher2");
  monsters[MONSTER_SMASHER2].moveMonster = moveSmasher;
  monsters[MONSTER_SMASHER2].drawMonster = drawSmasher;
  monsters[MONSTER_SMASHER2].start_speed_x = 4;
  monsters[MONSTER_SMASHER2].start_speed_y = 4;

  // demon monster
  strcpy(monsters[MONSTER_DEMON].name, "little demon");
  monsters[MONSTER_DEMON].moveMonster = moveDemon;
  monsters[MONSTER_DEMON].start_speed_x = 4;
  monsters[MONSTER_DEMON].start_speed_y = 4;
  // animation 2x slower
  monsters[MONSTER_DEMON].face_mod = 4;

  // platforms
  strcpy(monsters[MONSTER_PLATFORM].name, "platform");
  monsters[MONSTER_PLATFORM].moveMonster = movePlatform;
  monsters[MONSTER_PLATFORM].start_speed_x = 4;
  monsters[MONSTER_PLATFORM].start_speed_y = 4;
  monsters[MONSTER_PLATFORM].harmless = 1;

  // platform2
  strcpy(monsters[MONSTER_PLATFORM2].name, "platform2");
  monsters[MONSTER_PLATFORM2].moveMonster = movePlatform2;
  monsters[MONSTER_PLATFORM2].start_speed_x = 4;
  monsters[MONSTER_PLATFORM2].start_speed_y = 4;
  monsters[MONSTER_PLATFORM2].harmless = 1;

  // spider
  strcpy(monsters[MONSTER_SPIDER].name, "spider");
  monsters[MONSTER_SPIDER].moveMonster = moveSmasher;
  monsters[MONSTER_SPIDER].drawMonster = drawSmasher;
  monsters[MONSTER_SPIDER].start_speed_x = 2;
  monsters[MONSTER_SPIDER].start_speed_y = 2;

  // bear monster
  strcpy(monsters[MONSTER_BEAR].name, "arctic cave bear");
  monsters[MONSTER_BEAR].moveMonster = moveBear;
  monsters[MONSTER_BEAR].start_speed_x = 1;
  monsters[MONSTER_BEAR].start_speed_y = 1;
  monsters[MONSTER_BEAR].face_mod = 8;
  monsters[MONSTER_BEAR].random_speed = 0;

  // torch
  strcpy(monsters[MONSTER_TORCH].name, "torch");
  monsters[MONSTER_TORCH].moveMonster = moveTorch;
  monsters[MONSTER_TORCH].start_speed_x = 1;
  monsters[MONSTER_TORCH].start_speed_y = 1;
  monsters[MONSTER_TORCH].face_mod = 6;
  monsters[MONSTER_TORCH].harmless = 1;

  // arrow trap
  strcpy(monsters[MONSTER_ARROW].name, "arrow");
  monsters[MONSTER_ARROW].moveMonster = moveArrow;
  monsters[MONSTER_ARROW].start_speed_x = 1;
  monsters[MONSTER_ARROW].start_speed_y = 1;
  monsters[MONSTER_ARROW].face_mod = 1;

  // fire
  strcpy(monsters[MONSTER_FIRE].name, "fire");
  monsters[MONSTER_FIRE].moveMonster = moveFire;
  monsters[MONSTER_FIRE].drawMonster = drawFire;
  monsters[MONSTER_FIRE].start_speed_x = 2;
  monsters[MONSTER_FIRE].start_speed_y = 2;
  monsters[MONSTER_FIRE].face_mod = 3;

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
  if(m->random_speed) {
	live_monsters[live_monster_count].speed_x = m->start_speed_x + ((int)(MAX_RANDOM_SPEED * rand()/(RAND_MAX)));
	live_monsters[live_monster_count].speed_y = m->start_speed_y + ((int)(MAX_RANDOM_SPEED * rand()/(RAND_MAX)));
  } else {
	live_monsters[live_monster_count].speed_x = m->start_speed_x;
	live_monsters[live_monster_count].speed_y = m->start_speed_y;
  }
  live_monsters[live_monster_count].dir = DIR_NONE;
  live_monsters[live_monster_count].face = 0;
  live_monsters[live_monster_count].monster = m;
  live_monster_count++;
  fprintf(stderr, "Added live monster! monster=%s x=%d y=%d count=%d\n", 
		  m->name, x, y, live_monster_count);
  fflush(stderr);
}

void removeLiveMonster(int live_monster_index) {
  int t;
  LiveMonster *p;

  // debug
  if(live_monster_index >= live_monster_count) {
	fprintf(stderr, "Trying to remove monster w. index out of bounds: count=%d index=%d\n", 
			live_monster_count, live_monster_index);
	for(t = 0; t < live_monster_count; t++) {
	  fprintf(stderr, "\tmonster=%s x=%d y=%d\n", 
			  live_monsters[t].monster->name, live_monsters[t].pos_x, live_monsters[t].pos_y);
	}
	fflush(stderr);
	exit(-1);
  }

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

  fprintf(stderr, "Removed live monster! monster=%s index=%d count=%d\n", 
		  live_monsters[live_monster_index].monster->name, live_monster_index, live_monster_count);
  fflush(stderr);
}

void removeAllLiveMonsters() {
  while(live_monster_count > 0) {
	removeLiveMonster(0);
  }
}

/**
   Here rect is in pixels where 0, 0 is the screen's left top corner.
   Returns 0 for false and non-0 for true.
 */
int isOnScreen(SDL_Rect *rect) {
  return intersects(rect, &extended_screen_rect);
}

/**
   Draw all currently tracked creatures.
   start_x, start_y are the offset of the screen's top left edge in pixels.
 */
void drawLiveMonsters(SDL_Surface *surface, int start_x, int start_y) {
  SDL_Rect pos;
  SDL_Surface *img;
  int i;

  for(i = 0; i < live_monster_count; i++) {
	if(move_monsters)
	  live_monsters[i].monster->moveMonster(&live_monsters[i]);
	
	img = images[live_monsters[i].monster->image_index[getLiveMonsterFace(&live_monsters[i])]]->image;
	pos.x = live_monsters[i].pos_x * TILE_W - start_x + live_monsters[i].pixel_x;
	pos.y = live_monsters[i].pos_y * TILE_H - start_y + live_monsters[i].pixel_y;
	pos.w = img->w;
	pos.h = img->h;

	if(!isOnScreen(&pos)) {
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
   Return live monster if there's a one at position pos,
   NULL otherwise.
 */
LiveMonster *detectMonster(Position *pos) {
  int i;
  SDL_Rect monster, check;
  SDL_Surface *img;

  check.x = pos->pos_x * TILE_W + pos->pixel_x;
  check.y = pos->pos_y * TILE_H + pos->pixel_y;
  check.w = pos->w * TILE_W;
  check.h = pos->h * TILE_H;
  for(i = 0; i < live_monster_count; i++) {
	img = images[live_monsters[i].monster->image_index[getLiveMonsterFace(&live_monsters[i])]]->image;
	monster.x = live_monsters[i].pos_x * TILE_W + live_monsters[i].pixel_x;
	monster.y = live_monsters[i].pos_y * TILE_H + live_monsters[i].pixel_y;
	monster.w = img->w;
	monster.h = img->h;
	if(intersectsBy(&check, &monster, MONSTER_COLLISION_FUZZ)) {
	  return &live_monsters[i];
	}
  }


  return NULL;
}
