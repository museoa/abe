#include "Map.h"

void waitUntilPaintingStops();
void finishDrawMap();
int last_dir = -1;

// some private global variables used to draw the map
int screen_center_x, screen_center_y; // the screen's center in tiles
int screen_w, screen_h;               // the screen's size in tiles
typedef struct _mapDrawParams {
  int start_x; // where to start drawing the map
  int start_y;
  int end_x;   // where to stop drawing the map
  int end_y;
  int offset_x;  // how many tiles to offset from the top/left edge of the screen
  int offset_y;
} MapDrawParams;

/**
   Compute what to draw based on the cursor's location.
 */
void getMapDrawParams(MapDrawParams *params) {
  params->start_x = cursor.pos_x - screen_center_x;
  params->start_y = cursor.pos_y - screen_center_y;
  params->end_x = params->start_x + screen_w; 
  params->end_y = params->start_y + screen_h;
  int n;
  params->offset_x = 0;
  params->offset_y = 0;
  if(params->start_x < 0) {
	params->offset_x = -params->start_x;
	params->start_x = 0;
  } else {
	n = (params->start_x >= EXTRA_X ? EXTRA_X : params->start_x);
	params->offset_x = -n;
	params->start_x -= n;	
  }
  if(params->start_y < 0) {
	params->offset_y = -params->start_y;
	params->start_y = 0;	
  } else {
	n = (params->start_y >= EXTRA_Y ? EXTRA_Y : params->start_y);
	params->offset_y = -n;
	params->start_y -= n;	
  }
  if(params->end_x > map.w) params->end_x = map.w;
  if(params->end_y > map.h) params->end_y = map.h;

}

/**
   Draw the part of the map that appears on the screen if the cursor
   is the center of the screen.
 */
void drawMap() {
  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // draw the map
  SDL_Rect pos;
  int x, y, level, n;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {

	// erase the screen
	pos.x = 0;
	pos.y = 0;
	pos.w = map.level[level]->w;
	pos.h = map.level[level]->h;
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x0, 0x0, 0x0, 0x00));
	}

	for(y = params.start_y; y <= params.end_y && params.end_y < map.h; y++) {
	  for(x = params.start_x; x <= params.end_x && params.end_x < map.w;) {
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  pos.x = (params.offset_x + (x - params.start_x)) * TILE_W;
		  pos.y = (params.offset_y + (y - params.start_y)) * TILE_H;
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  pos.x += (EXTRA_X * TILE_W) - cursor.pixel_x;
		  pos.y += (EXTRA_Y * TILE_H) - cursor.pixel_y;
		  SDL_BlitSurface(images[n]->image, NULL, map.level[level], &pos);

		  // skip ahead
		  x += images[n]->image->w / TILE_W;
		} else {	  
		  x++;
		}
	  }
	}
  }
  finishDrawMap();
}

/**
   Draw only the left first column of the map.
   This is the first column which is displayed on the virtual screens
   given that the cursor is in the middle of the screen. Note that this
   column will be off-screen since the virtual screens extend offscreen to
   the left and top by EXTRA_X and EXTRA_Y tiles.
 */
void drawMapLeftEdge() {
  // erase the edge
  SDL_Rect pos;
  int n, row, level, x, y;
  pos.x = 0;
  pos.y = 0;
  pos.w = cursor.speed_x;
  pos.h = map.level[0]->h;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }

  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the left edge param
  params.start_x = (cursor.pos_x - screen_center_x) - EXTRA_X;
   
  
  // redraw the left edge of the screen
  if(params.start_x >= 0) {
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  for(y = params.start_y; y < params.end_y; y++) {
		for(x = params.start_x; x <= params.start_x + 1;) {
		  n = (map.image_index[level][x + (y * map.w)]);
		  if(n > -1) {
			// Draw the image
			// should be some check here in case images[n] points to la-la-land.
			//pos.x = (offset_x + (x - start_x)) * TILE_W;
			pos.x = ((x - params.start_x) * TILE_W) - cursor.pixel_x;
			pos.y = (params.offset_y + (y - params.start_y)) * TILE_H;
			pos.w = images[n]->image->w;
			pos.h = images[n]->image->h;
			
			// compensate for extra area
			//pos.x += (EXTRA_X * TILE_W) - cursor.pixel_x;
			pos.y += (EXTRA_Y * TILE_H) - cursor.pixel_y;
			SDL_BlitSurface(images[n]->image, NULL, map.level[level], &pos);
			// skip ahead
			x += images[n]->image->w / TILE_W;
		  } else {
			x++;
		  }
		}
	  }
	}
  }
}

/**
   Draw only the top first row of the map.
   This is the first row which is displayed on the virtual screens
   given that the cursor is in the middle of the screen. Note that this
   row will be off-screen since the virtual screens extend offscreen to
   the left and top by EXTRA_X and EXTRA_Y tiles.
 */
void drawMapTopEdge() {
  // erase the edge
  SDL_Rect pos;
  int n, row, level, x, y;
  pos.x = 0;
  pos.y = 0;
  pos.w = map.level[0]->w;
  pos.h = cursor.speed_y;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }

  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the top edge param
  params.start_y = (cursor.pos_y - screen_center_y) - EXTRA_Y;
   
  if(params.start_y >= 0) {
	// redraw the left edge of the screen
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  for(y = params.start_y; y <= params.start_y + 1; y++) {
		for(x = params.start_x; x < params.end_x;) {
		  n = (map.image_index[level][x + (y * map.w)]);
		  if(n > -1) {
			// Draw the image
			// should be some check here in case images[n] points to la-la-land.
			pos.x = (params.offset_x + (x - params.start_x)) * TILE_W;
			pos.y = ((y - params.start_y) * TILE_H)-cursor.pixel_y;
			pos.w = images[n]->image->w;
			pos.h = images[n]->image->h;
			
			// compensate for extra area
			pos.x += (EXTRA_X * TILE_W) - cursor.pixel_x;
			//pos.y += (EXTRA_Y * TILE_H) - cursor.pixel_y;
			SDL_BlitSurface(images[n]->image, NULL, map.level[level], &pos);
			// skip ahead
			x += images[n]->image->w / TILE_W;
		  } else {
			x++;
		  }
		}
	  }
	}
  }
}

/**
   Draw only the right last column of the map.
   This is the last column which is displayed on the virtual screens
   given that the cursor is in the middle of the screen.
 */
void drawMapRightEdge() {
  // erase the edge
  SDL_Rect pos;
  int n, row, level, x, y;
  pos.x = map.level[0]->w - cursor.speed_x;
  pos.y = 0;
  pos.w = cursor.speed_x;
  pos.h = map.level[0]->h;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }

  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the right edge param
  params.end_x = (cursor.pos_x - screen_center_x) + screen_w;

  // redraw the left edge of the screen
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	for(y = params.start_y; y < params.end_y; y++) {
	  // here we have to draw more than 1 column b/c images
	  // extend from right_edge-EXTRA_X on. 
	  for(x = params.end_x - EXTRA_X; x <= params.end_x;) {
		if(params.end_x >= map.w) break;
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  //pos.x = (offset_x + (x - start_x)) * TILE_W;
		  pos.x = (map.level[level]->w - ((params.end_x - x) * TILE_W)) - cursor.pixel_x;
		  pos.y = (params.offset_y + (y - params.start_y)) * TILE_H;
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  //pos.x += (EXTRA_X * TILE_W) - cursor.pixel_x;
		  pos.y += (EXTRA_Y * TILE_H) - cursor.pixel_y;
		  SDL_BlitSurface(images[n]->image, NULL, map.level[level], &pos);
		  // skip ahead
		  x += images[n]->image->w / TILE_W;
		} else {
		  x++;
		}
	  }
	}
  }
}

/**
   Draw only the right last column of the map.
   This is the last column which is displayed on the virtual screens
   given that the cursor is in the middle of the screen.
 */
void drawMapBottomEdge() {
  // erase the edge
  SDL_Rect pos;
  int n, row, level, x, y;
  pos.x = 0;
  pos.y = map.level[0]->h - cursor.speed_y;
  pos.w = map.level[0]->w;
  pos.h = cursor.speed_y;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }

  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the bottom edge param
  params.end_y = (cursor.pos_y - screen_center_y) + screen_h;
   
  // redraw the left edge of the screen
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	// here we have to draw more than 1 column b/c images
	// extend from right_edge-EXTRA_X on. 
	for(y = params.end_y - EXTRA_Y; y <= params.end_y; y++) {
	  if(params.end_y >= map.h) break;
	  for(x = params.start_x; x < params.end_x;) {
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  pos.x = (params.offset_x + (x - params.start_x)) * TILE_W;
		  pos.y = (map.level[level]->h - ((params.end_y - y) * TILE_H)) - cursor.pixel_y;
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  pos.x += (EXTRA_X * TILE_W) - cursor.pixel_x;
		  //pos.y += (EXTRA_Y * TILE_H) - cursor.pixel_y;
		  SDL_BlitSurface(images[n]->image, NULL, map.level[level], &pos);
		  // skip ahead
		  x += images[n]->image->w / TILE_W;
		} else {
		  x++;
		}
	  }
	}
  }
}

/**
   Scrolls but doesn't update the screen.
   You must call finishDrawMap() after this method.
   (this is so you can string together multiple calls to this method.)
*/
void scrollMap(int dir) {
  if(dir == DIR_NONE) return;

  // move the screen
  int row, level;
  switch(dir) {
  case DIR_LEFT:

	// scroll the virtual screens right row by row
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  if(SDL_LockSurface(map.level[level]) == -1) {
		fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		fflush(stderr);
		exit(0);	
	  }
	  for(row = 0; row < map.level[level]->h; row++) {
		memmove((Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row) + (long)cursor.speed_x), 
				(Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row)),
				(long)(map.level[level]->w - cursor.speed_x) * (long)sizeof(Uint16));
	  }
	  SDL_UnlockSurface(map.level[level]);
	}

	// draw only the new left edge
	drawMapLeftEdge();

	break;


  case DIR_RIGHT:

	// scroll the virtual screens left row by row
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  if(SDL_LockSurface(map.level[level]) == -1) {
		fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		fflush(stderr);
		exit(0);	
	  }
	  for(row = 0; row < map.level[level]->h; row++) {
		// I guess this could be a memcpy for left and up...
		memmove((Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row)), 
				(Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row + (long)cursor.speed_x)),
				(long)(map.level[level]->w - cursor.speed_x) * (long)sizeof(Uint16));
	  }
	  SDL_UnlockSurface(map.level[level]);
	}

	// draw only the new left edge
	drawMapRightEdge();
	
	break;


  case DIR_UP:

	// scroll the virtual screens down
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  if(SDL_LockSurface(map.level[level]) == -1) {
		fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		fflush(stderr);
		exit(0);	
	  }
	  long skipped = (long)map.level[level]->w * (long)cursor.speed_y;
	  memmove((Uint16*)((Uint16*)(map.level[level]->pixels) + skipped), 
			  (Uint16*)(map.level[level]->pixels),
			  (long)(map.level[level]->w * map.level[level]->h - skipped) * (long)sizeof(Uint16));
	  SDL_UnlockSurface(map.level[level]);
	}

	// draw only the new left edge
	drawMapTopEdge();

	break;


  case DIR_DOWN:

	// scroll the virtual screens up
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  if(SDL_LockSurface(map.level[level]) == -1) {
		fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		fflush(stderr);
		exit(0);	
	  }
	  long skipped = (long)map.level[level]->w * (long)cursor.speed_y;
	  memmove((Uint16*)(map.level[level]->pixels), 
			  (Uint16*)((Uint16*)(map.level[level]->pixels) + skipped),
			  (long)(map.level[level]->w * map.level[level]->h - skipped) * (long)sizeof(Uint16));
	  SDL_UnlockSurface(map.level[level]);
	}

	// draw only the new left edge
	drawMapBottomEdge();

	break;
  }
}

int moveLeft(int checkCollision, int scroll) {
  int move, old_pixel, old_pos, old_speed;
  old_speed = cursor.speed_x;
  //  while(cursor.speed_x > 0) {
  while(1) {
	move = 1;
	old_pixel = cursor.pixel_x;
	old_pos = cursor.pos_x;
	
	cursor.pixel_x -= cursor.speed_x;
	if(cursor.pixel_x < 0) {
	  cursor.pixel_x += TILE_W;
	  cursor.pos_x--;
	  if(cursor.pos_x < 0) {
		move = 0;
	  }
	}
	if(move && (!checkCollision || map.detectCollision(DIR_LEFT))) {
	  if(scroll) scrollMap(DIR_LEFT);	
	  if(map.accelerate) {
		if(cursor.speed_x < TILE_W) {
		  cursor.speed_x += SPEED_INC_X;
		  if(cursor.speed_x >= TILE_W) cursor.speed_x = TILE_W;
		}
	  }
	  //cursor.speed_x = old_speed;
	  return 1;
	}
	cursor.pixel_x = old_pixel;
	cursor.pos_x = old_pos;	
	//cursor.speed_x -= SPEED_INC_X;
	if(!cursor.pixel_x) break;
	cursor.speed_x = cursor.pixel_x;
  }
  //  cursor.dir = DIR_NONE;
  //cursor.jump = 0;
  cursor.speed_x = old_speed;
  return 0;
}

int moveRight(int checkCollision, int scroll) {
  int move, old_pixel, old_pos, old_speed;
  old_speed = cursor.speed_x;
  //  while(cursor.speed_x > 0) {
  while(1) {
	move = 1;
	old_pixel = cursor.pixel_x;
	old_pos = cursor.pos_x;

	cursor.pixel_x += cursor.speed_x;
	if(cursor.pixel_x >= TILE_W) {
	  cursor.pixel_x -= TILE_W;
	  cursor.pos_x++;
	  if(cursor.pos_x >= map.w) {
		move = 0;
	  }
	}
	if(move && (!checkCollision || map.detectCollision(DIR_RIGHT))) {
	  if(scroll) scrollMap(DIR_RIGHT);	
	  if(map.accelerate) {
		if(cursor.speed_x < TILE_W) {
		  cursor.speed_x += SPEED_INC_X;
		  if(cursor.speed_x >= TILE_W) cursor.speed_x = TILE_W;
		}
	  }
	  //cursor.speed_x = old_speed;
	  return 1;
	}
	cursor.pixel_x = old_pixel;
	cursor.pos_x = old_pos;
	//cursor.speed_x -= SPEED_INC_X;
	if(!cursor.pixel_x) break;
	cursor.speed_x = TILE_W - cursor.pixel_x;
  }
  //  cursor.dir = DIR_NONE;
  //cursor.jump = 0;
  cursor.speed_x = old_speed;
  return 0;
}

int moveUp(int checkCollision, int scroll) {
  int move, old_pixel, old_pos, old_speed;
  old_speed = cursor.speed_y;
  while(cursor.speed_y > 0) {
	move = 1;
	old_pixel = cursor.pixel_y;
	old_pos = cursor.pos_y;

	cursor.pixel_y -= cursor.speed_y;
	if(cursor.pixel_y < 0) {
	  cursor.pixel_y += TILE_H;
	  cursor.pos_y--;
	  if(cursor.pos_y < 0) {
		move = 0;
	  }
	}
	if(move && (!checkCollision || 
				((cursor.jump || map.detectLadder() || cursor.stepup) && map.detectCollision(DIR_UP)))) {
	  if(scroll) scrollMap(DIR_UP);	
	  if(map.accelerate) {
		if(cursor.speed_y < TILE_H) {
		  cursor.speed_y += SPEED_INC_Y;
		  if(cursor.speed_y >= TILE_H) cursor.speed_y = TILE_H;
		}
	  }
	  //	  cursor.speed_y = old_speed;
	  return 1;
	}
	cursor.pixel_y = old_pixel;
	cursor.pos_y = old_pos;
	cursor.speed_y -= SPEED_INC_Y;
  }
  //cursor.dir = DIR_NONE;
  //cursor.jump = 0;
  cursor.speed_y = old_speed;
  return 0;
}

int moveDown() {
  int move, old_pixel, old_pos, old_speed;
  old_speed = cursor.speed_y;
  while(cursor.speed_y > 0) {
	move = 1;
	old_pixel = cursor.pixel_y;
	old_pos = cursor.pos_y;

	cursor.pixel_y += cursor.speed_y;
	if(cursor.pixel_y >= TILE_H) {
	  cursor.pixel_y -= TILE_H;
	  cursor.pos_y++;
	  if(cursor.pos_y >= map.h) {
		move = 0;
	  }
	}
	if(move && 
	   ((cursor.gravity || map.detectLadder()) && map.detectCollision(DIR_DOWN))) {
	  scrollMap(DIR_DOWN);	
	  if(map.accelerate) {
		if(cursor.speed_y < TILE_H) {
		  cursor.speed_y += SPEED_INC_Y;
		  if(cursor.speed_y >= TILE_H) cursor.speed_y = TILE_H;
		}
	  }
	  //	  cursor.speed_y = old_speed;
	  return 1;
	}
	cursor.pixel_y = old_pixel;
	cursor.pos_y = old_pos;
	cursor.speed_y -= SPEED_INC_Y;
  }
  //cursor.dir = DIR_NONE;
  //cursor.jump = 0;
  cursor.speed_y = old_speed;
  return 0;
}

/**
   Is there ground beneath tom?
   if so, return 1, 0 otherwise
 */
int onSolidGround() {
  //int old_dir = cursor.dir;
  int old_speed = cursor.speed_y;
  int old_pixel = cursor.pixel_y;
  int old_pos = cursor.pos_y;

  //  cursor.dir = DIR_DOWN;
  cursor.speed_y = (cursor.pixel_y == 0 ? TILE_H : cursor.pixel_y);
  int n = moveDown(1, 0);
  
  //cursor.dir = old_dir;
  cursor.speed_y = old_speed;
  cursor.pixel_y = old_pixel;
  cursor.pos_y = old_pos;

  // if you can't move down then we're on ground.
  return !n;
}

/**
   When moving left or right and hitting an obsticle, call this method
   to check if we can step onto the object.
   Returns 1 on success, 0 on failure.
 */
int canStepUp(int dir) {
  // Take an unchecked step into the wall and don't scroll the screen.
  // This is so after stepping up, gravity won't pull us back down.
  // TILE_W b/c we always stop on pixel_x==0			
  cursor.speed_x = TILE_W;
  //cursor.dir = dir;
  if(dir == DIR_LEFT) {
	moveLeft(0, 1);
  } else {
	moveRight(0, 1);
  }
  
  // take a step up
  //cursor.dir = DIR_UP;
  cursor.speed_y = (cursor.pixel_y == 0 ? TILE_H : cursor.pixel_y);
  cursor.stepup = 1;
  //  if(!(moveUp(1, 0) && onSolidGround())) {
  if(!moveUp(1, 1)) {
	cursor.stepup = 0;
	// if can't step up
	cursor.speed_x = TILE_W;
	//cursor.dir = dir;
	if(dir == DIR_LEFT) {
	  moveRight(0, 1);
	} else {
	  moveLeft(0, 1);
	}
	cursor.speed_x = 0;

	//cursor.dir = DIR_NONE;
	//cursor.jump = 0;
	return 0;
  } else {
	cursor.stepup = 0;
	/*
	// scroll the screen up
	scrollMap(DIR_UP);
	*/
	// keep going right
	//cursor.dir = dir;
	// scroll the screen
	//scrollMap(dir);
	return 1;
  }
}

int moveJump() {
  int ret = 0;
  if(cursor.jump > 0) {
	cursor.jump--;

	//int old_dir = cursor.dir;
	int old_speed = cursor.speed_y;
	//cursor.dir = DIR_UP;
	cursor.speed_y = JUMP_SPEED;
	moveUp(1, 1);
	//cursor.dir = old_dir;
	cursor.speed_y = old_speed;
	ret = 1;
  }
  return ret;
}

/**
   Drop down.
 */
void moveGravity() {
  if(map.gravity) {
	if(map.detectLadder()) return;
	//int old_dir = cursor.dir;
	int old_speed = cursor.speed_y;
	//cursor.dir = DIR_DOWN;
	cursor.speed_y = 10;
	cursor.gravity = 1;
	moveDown();
	cursor.gravity = 0;
	//cursor.dir = (old_dir == DIR_NONE ? DIR_DOWN : old_dir);
	cursor.speed_y = old_speed;
  }
}

/**
   The main thread loop
 */
int moveMap() {
  SDL_Event event;
  int delay;
  while(!map.stopThread) {

	// handle events.
	while(SDL_PollEvent(&event)) {
	  map.handleMapEvent(&event);
	}
	// jumping
	if(!moveJump()) {
	  // activate gravity
	  moveGravity();
	}
	// set unaccelerated speed
	if(!map.accelerate) {
	  cursor.speed_x = TILE_W;
	  cursor.speed_y = TILE_H;
	}
	switch(cursor.dir) {
	case DIR_QUIT:
	  return;
	case DIR_LEFT:
	  if(!moveLeft(1, 1)) {
		// try to step up onto the obsticle
		canStepUp(DIR_LEFT);
	  }
	  break;	
	case DIR_RIGHT:
	  if(!moveRight(1, 1)) {
		// try to step up onto the obsticle
		canStepUp(DIR_RIGHT);
	  }
	  break;	
	case DIR_UP:
	  moveUp(1, 1);
	  break;
	case DIR_DOWN:
	  moveDown();
	  break;
	case DIR_UPDATE:
	  cursor.dir = DIR_NONE;
	  break;
	}

	// update the screen
	finishDrawMap();

	if(cursor.wait) {
	  cursor.wait = 0;
	  delay = map.delay;
	} else {
	  delay = 10; // a small delay to not max out the cpu
	}
	SDL_Delay(delay);
  }
}

void finishDrawMap() {
  // draw on screen
  SDL_Rect pos;
  int level;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	pos.x = -(EXTRA_X * TILE_W);
	pos.y = -(EXTRA_Y * TILE_H);
	pos.w = map.level[level]->w;
	pos.h = map.level[level]->h;
	SDL_BlitSurface(map.level[level], NULL, screen, &pos);
	// make a callback if it exists
	if(level == LEVEL_MAIN && map.afterMainLevelDrawn) {
	  map.afterMainLevelDrawn();
	}
  }

  // if the callback function is set, call it now.
  if(map.beforeDrawToScreen) {
	map.beforeDrawToScreen();
  }
    
  SDL_Flip(screen);
}

void setImage(int level, int index) {
  int x, y, n;
  // clear the area 
  SDL_Rect rect, img_rect;
  if(index > -1) {
	img_rect.x = cursor.pos_x;
	img_rect.y = cursor.pos_y;
	img_rect.w = images[index]->image->w / TILE_W;
	img_rect.h = images[index]->image->h / TILE_H;
  }
  int start_x = cursor.pos_x - EXTRA_X;
  if(start_x < 0) start_x = 0;
  int start_y = cursor.pos_y - EXTRA_Y;
  if(start_y < 0) start_y = 0;
  int end_x = cursor.pos_x + (index > -1 ? images[index]->image->w / TILE_W : 1);
  if(end_x >= map.w) end_x = map.w;
  int end_y = cursor.pos_y + (index > -1 ? images[index]->image->h / TILE_H : 1);
  if(end_y >= map.h) end_y = map.h;
  for(y = start_y; y < end_y; y++) {
	for(x = start_x; x < end_x; x++) {
	  n = map.image_index[level][x + (y * map.w)];
	  if(n > -1) {
		rect.x = x;
		rect.y = y;
		rect.w = images[n]->image->w / TILE_W;
		rect.h = images[n]->image->h / TILE_H;
		if(contains(&rect, cursor.pos_x, cursor.pos_y) || 
		   (index > -1 && intersects(&rect, &img_rect))) {	  
		  map.image_index[level][x + (y * map.w)] = -1;
		}
	  }
	}
  }
  // add the image
  map.image_index[level][cursor.pos_x + (cursor.pos_y * map.w)] = index;
  // move the cursor
  if(index > -1) {
	if(cursor.pos_x + (images[index]->image->w / TILE_W) < map.w) 
	  cursor.pos_x += (images[index]->image->w / TILE_W);
  }
  drawMap();
}

int defaultDetectCollision(int dir) {
  return 1;
}

int defaultDetectLadder() {
  return 1;
}

void initMap(char *name, int w, int h) {
  // start a new Map
  map.gravity = 0;
  map.accelerate = 0;
  map.name = strdup(name);
  map.w = w;
  map.h = h;
  map.beforeDrawToScreen = NULL;
  map.afterMainLevelDrawn = NULL;
  map.detectCollision = defaultDetectCollision;
  map.detectLadder = defaultDetectLadder;
  map.handleMapEvent = NULL;
  map.delay = 25;
  int i;
  for(i = LEVEL_BACK; i < LEVEL_COUNT; i++) {
	if(!(map.image_index[i] = malloc(sizeof(int) * w * h))) {
	  fprintf(stderr, "Out of memory.\n");
	  fflush(stderr);
	  exit(0);
	}
	if(!(map.level[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, 
											  screen->w + EXTRA_X * TILE_W, 
											  screen->h + EXTRA_Y * TILE_H, 
											  screen->format->BitsPerPixel, 
											  0, 0, 0, 0))) {
	  fprintf(stderr, "Error creating surface: %s\n", SDL_GetError());
	  fflush(stderr);
	  return;
	}
	// set black as the transparent color key
	if(i > LEVEL_BACK) {
	  SDL_SetColorKey(map.level[i], SDL_SRCCOLORKEY, SDL_MapRGBA(map.level[i]->format, 0x00, 0x00, 0x00, 0xff));
	}
  }
  // clean map
  for(i = 0; i < (w * h); i++) {
	map.image_index[LEVEL_BACK][i] = -1;
	map.image_index[LEVEL_MAIN][i] = -1;
	map.image_index[LEVEL_FORE][i] = -1;
  }

  // init some variables
  screen_center_x = (screen->w / TILE_W) / 2;
  screen_center_y = (screen->h / TILE_H) / 2;
  screen_w = screen->w / TILE_W;
  //  screen_h = (screen->h - edit_panel.image->h) / TILE_H;
  screen_h = screen->h / TILE_H;
}

void destroyMap() {
  free(map.name);
  int i;
  for(i = LEVEL_BACK; i < LEVEL_COUNT; i++) {
	SDL_FreeSurface(map.level[i]);
	free(map.image_index[i]);
  }
}

void resetCursor() {
  cursor.pos_x = 0;
  cursor.pos_y = 0;
  cursor.pixel_x = 0;
  cursor.pixel_y = 0;
  cursor.speed_x = TILE_W;
  cursor.speed_y = TILE_H;
  cursor.dir = DIR_NONE;
  cursor.wait = 0;
  cursor.jump = 0;
  cursor.gravity = 0;
  cursor.stepup = 0;
}

void saveMap() {
  char path[300];
  sprintf(path, "%s/%s.dat", MAPS_DIR, map.name);
  printf("Saving map %s\n", path);  
  fflush(stdout);
  FILE *fp;
  if(!(fp = fopen(path, "wb"))) {
	fprintf(stderr, "Can't open file for writing.");
	fflush(stderr);
	return;
  }
  // write the header
  fwrite(&(map.w), sizeof(map.w), 1, fp);
  fwrite(&(map.h), sizeof(map.h), 1, fp);

  // compression step 1
  size_t new_size;
  printf("Compressing...\n");
  int *compressed_map = compressMap(&new_size);
  fprintf(stderr, "Compressed map. old_size=%ld new_size=%ld\n", (LEVEL_COUNT * map.w * map.h), new_size);
  fflush(stderr);
  // write out and further compress in step 2
  size_t written = compress(compressed_map, new_size, fp);
  fprintf(stderr, "Compressed map step2. Written %ld ints. Compression ration: %f.2\%\n", written, 
		  (float)written / ((float)(LEVEL_COUNT * map.w * map.h) / 100.0));
  fflush(stderr);
  fclose(fp);
  free(compressed_map);
}

// call this after initMap()!
int loadMap(int draw_map) {
  char path[300];
  sprintf(path, "%s/%s.dat", MAPS_DIR, map.name);
  printf("Loading map %s\n", path);  
  fflush(stdout);
  FILE *fp;
  if(!(fp = fopen(path, "rb"))) {
	fprintf(stderr, "Can't open file for reading.");
	fflush(stderr);
	return 0;
  }
  // read the header
  fread(&(map.w), sizeof(map.w), 1, fp);
  fread(&(map.h), sizeof(map.h), 1, fp);

  // compression step 1: read compressed data from disk
  // FIXME: what would be nicer is to only allocate as much mem as used on disk.
  size_t size = LEVEL_COUNT * map.w * map.h;
  int *read_buff;
  if(!(read_buff = malloc(sizeof(int) * size))) {
	fprintf(stderr, "Out of memory on map read.");
	fflush(stderr);
	exit(0);
  }
  int count_read = decompress(read_buff, size, fp);
  fprintf(stderr, "read %d ints\n", count_read);
  fflush(stderr);
  fclose(fp);
  
  // step 2: further uncompress
  decompressMap(read_buff);
  free(read_buff);

  resetCursor();
  if(draw_map) drawMap();
  return 1;
}

/** Remove unnecesary -1s. For example a 4 tile wide stone becomes a 1 int number.
	return new number of elements in new_size. (so num of bytes=new_size * sizeof(int)).
	caller must free returned pointer.
	call this method before calling Utils.compress(). This prepares the map
	for better compression by understanding the its structure. This doesn't 
	compress the map that much, but combined with Utils.compress() map files
	can go from 12M to 14K!
*/
int *compressMap(size_t *new_size) {
  int *q;
  if(!(q = malloc(sizeof(int) * map.w * map.h * LEVEL_COUNT))) {
	fprintf(stderr, "Out of memory in compressMap.");
	fflush(stderr);
	exit(-1);
  }
  int level, i, x, y, n;
  size_t t = 0;
  for(level = 0; level < LEVEL_COUNT; level++) {
	for(y = 0; y < map.h; y++) {
	  for(x = 0; x < map.w;) {
		i = x + (y * map.w);
		n = map.image_index[level][i];
		*(q + t) = n;
		t++;
		if(n > -1) {
		  // skip ahead
		  x += images[n]->image->w / TILE_W;
		} else {
		  x++;
		}
	  }
	}
  }
  *new_size = t;
  return q;
}

/**
   Decompress map by adding back missing -1-s. See compressMap() for
   details.
 */
void decompressMap(int *p) {
  int level, i, x, y, n, r, nn;
  size_t t = 0;
  for(level = 0; level < LEVEL_COUNT; level++) {
	for(y = 0; y < map.h; y++) {
	  for(x = 0; x < map.w;) {
		n = *(p + t);
		t++;
		i = x + (y * map.w);
		map.image_index[level][i] = n;
		x++;
		if(n > -1) {
		  for(r = 1; r < images[n]->image->w / TILE_W && x < map.w; r++, x++) {
			map.image_index[level][i + r] = -1;
		  }
		}
	  }
	}
  }
}

void startJump() {
  if(!cursor.jump) {
	cursor.jump = JUMP_LENGTH;
  }
}
