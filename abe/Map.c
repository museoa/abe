#include <errno.h>
#include "Map.h"

typedef struct _gameCollisionCheck {
  int start_x, start_y, end_x, end_y;
  SDL_Rect rect;
} GameCollisionCheck;


void waitUntilPaintingStops();
void finishDrawMap();
int last_dir = -1;
// some private global variables used to draw the map
int screen_center_x, screen_center_y; // the screen's center in tiles
int screen_w, screen_h;               // the screen's size in tiles

/** 
	Which section of the screen is visible
*/
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
  int n;

  map.top_left_x = cursor.pos_x - screen_center_x;
  map.top_left_y = cursor.pos_y - screen_center_y;

  params->start_x = cursor.pos_x - screen_center_x;
  params->start_y = cursor.pos_y - screen_center_y;
  params->end_x = params->start_x + screen_w; 
  params->end_y = params->start_y + screen_h;

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
  SDL_Rect pos;
  int x, m, y, level, n;
  MapDrawParams params;

  // compute what to draw
  getMapDrawParams(&params);

  // draw the map
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
		n = map.image_index[level][x + (y * map.w)];
		if(map.monsters) {
		  m = isMonsterImage(n);
		  if(m > -1) {
			map.image_index[level][x + (y * map.w)] = EMPTY_MAP;
			addLiveMonster(m, n, x, y);
			continue;
		  }
		}
		if(n != EMPTY_MAP) {
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
  int n, m, row, level, x, y;
  MapDrawParams params;

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
  getMapDrawParams(&params);

  // override the left edge param
  params.start_x = (cursor.pos_x - screen_center_x) - EXTRA_X;
   
  
  // redraw the left edge of the screen
  if(params.start_x >= 0) {
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  for(y = params.start_y; y < params.end_y; y++) {
		for(x = params.start_x; x <= params.start_x + 1;) {
		  n = (map.image_index[level][x + (y * map.w)]);
		  if(map.monsters) {
			m = isMonsterImage(n);
			if(m > -1) {
			  map.image_index[level][x + (y * map.w)] = EMPTY_MAP;
			  addLiveMonster(m, n, x, y);
			  continue;
			}
		  }
		  if(n != EMPTY_MAP) {
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
  int n, m, row, level, x, y;
  MapDrawParams params;

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
  getMapDrawParams(&params);

  // override the top edge param
  params.start_y = (cursor.pos_y - screen_center_y) - EXTRA_Y;
   
  if(params.start_y >= 0) {
	// redraw the left edge of the screen
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  for(y = params.start_y; y <= params.start_y + 1; y++) {
		for(x = params.start_x; x < params.end_x;) {
		  n = (map.image_index[level][x + (y * map.w)]);
		  if(map.monsters) {
			m = isMonsterImage(n);
			if(m > -1) {
			  map.image_index[level][x + (y * map.w)] = EMPTY_MAP;
			  addLiveMonster(m, n, x, y);
			  continue;
			}
		  }
		  if(n != EMPTY_MAP) {
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
  int n, m, row, level, x, y;
  MapDrawParams params;

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
		if(map.monsters) {
		  m = isMonsterImage(n);
		  if(m > -1) {
			map.image_index[level][x + (y * map.w)] = EMPTY_MAP;
			addLiveMonster(m, n, x, y);
			continue;
		  }
		}
		if(n != EMPTY_MAP) {
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
  int n, m, row, level, x, y;
  MapDrawParams params;

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
		if(map.monsters) {
		  m = isMonsterImage(n);
		  if(m > -1) {
			map.image_index[level][x + (y * map.w)] = EMPTY_MAP;
			addLiveMonster(m, n, x, y);
			continue;
		  }
		}
		if(n != EMPTY_MAP) {
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
  int row, level;
  long skipped;
  SDL_Rect pos;

  if(dir == DIR_NONE) return;

  // move the screen
  switch(dir) {
  case DIR_LEFT:

	if(FORCE_SDL_SCROLL || screen->flags & SDL_HWSURFACE) {
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		SDL_FillRect(map.transfer, NULL, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
		SDL_BlitSurface(map.level[level], NULL, map.transfer, NULL);
		pos.x = cursor.speed_x;
		pos.y = 0;
		pos.w = map.level[level]->w - cursor.speed_x;
		pos.h = map.level[level]->h;
		SDL_BlitSurface(map.transfer, NULL, map.level[level], &pos);
	  }
	} else {
	  // scroll the virtual screens right row by row
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		if(SDL_LockSurface(map.level[level]) == -1) {
		  fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		  fflush(stderr);
		  exit(0);	
		}
		for(row = 0; row < map.level[level]->h; row++) {
		  memmove((Uint8*)((Uint8*)(map.level[level]->pixels) + 
						   ((long)map.level[level]->pitch * (long)row) + 
						   (long)cursor.speed_x * map.level[level]->format->BytesPerPixel), 
				  (Uint8*)((Uint8*)(map.level[level]->pixels) + 
						   ((long)map.level[level]->pitch * (long)row)),
				  (long)(map.level[level]->pitch - (cursor.speed_x * map.level[level]->format->BytesPerPixel)));
		}
		SDL_UnlockSurface(map.level[level]);
	  }
	}

	// draw only the new left edge
	drawMapLeftEdge();

	break;


  case DIR_RIGHT:

	if(FORCE_SDL_SCROLL || screen->flags & SDL_HWSURFACE) {
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		SDL_FillRect(map.transfer, NULL, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
		pos.x = cursor.speed_x;
		pos.y = 0;
		pos.w = map.level[level]->w - cursor.speed_x;
		pos.h = map.level[level]->h;
		SDL_BlitSurface(map.level[level], &pos, map.transfer, NULL);
		pos.x = 0;
		pos.y = 0;
		pos.w = map.level[level]->w - cursor.speed_x;
		pos.h = map.level[level]->h;
		SDL_BlitSurface(map.transfer, NULL, map.level[level], &pos);
	  }
	} else {

	  // scroll the virtual screens left row by row
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		if(SDL_LockSurface(map.level[level]) == -1) {
		  fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		  fflush(stderr);
		  exit(0);	
		}
		for(row = 0; row < map.level[level]->h; row++) {
		  memmove((Uint8*)((Uint8*)(map.level[level]->pixels) + 
						   ((long)map.level[level]->pitch * (long)row)), 
				  (Uint8*)((Uint8*)(map.level[level]->pixels) + 
						   ((long)map.level[level]->pitch * (long)row + 
							(long)cursor.speed_x * map.level[level]->format->BytesPerPixel)),
				  (long)(map.level[level]->pitch - (long)(cursor.speed_x *  map.level[level]->format->BytesPerPixel)));
		}
		SDL_UnlockSurface(map.level[level]);
	  }
	}

	// draw only the new left edge
	drawMapRightEdge();
	
	break;


  case DIR_UP:
	if(FORCE_SDL_SCROLL || screen->flags & SDL_HWSURFACE) {
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		SDL_FillRect(map.transfer, NULL, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
		pos.x = 0;
		pos.y = 0;
		pos.w = map.level[level]->w;
		pos.h = map.level[level]->h - cursor.speed_y;
		SDL_BlitSurface(map.level[level], &pos, map.transfer, NULL);
		pos.x = 0;
		pos.y = cursor.speed_y;
		pos.w = map.level[level]->w;
		pos.h = map.level[level]->h - cursor.speed_y;
		SDL_BlitSurface(map.transfer, NULL, map.level[level], &pos);
	  }
	} else {
	  // scroll the virtual screens down
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		if(SDL_LockSurface(map.level[level]) == -1) {
		  fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		  fflush(stderr);
		  exit(0);	
		}
		skipped = (long)map.level[level]->pitch * (long)cursor.speed_y;
		memmove((Uint8*)((Uint8*)(map.level[level]->pixels) + skipped), 
				(Uint8*)(map.level[level]->pixels),
				(long)(map.level[level]->pitch * map.level[level]->h - skipped));
		SDL_UnlockSurface(map.level[level]);
	  }
	}

	// draw only the new left edge
	drawMapTopEdge();

	break;


  case DIR_DOWN:

	if(FORCE_SDL_SCROLL || screen->flags & SDL_HWSURFACE) {
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		SDL_FillRect(map.transfer, NULL, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
		pos.x = 0;
		pos.y = cursor.speed_y;
		pos.w = map.level[level]->w;
		pos.h = map.level[level]->h - cursor.speed_y;
		SDL_BlitSurface(map.level[level], &pos, map.transfer, NULL);
		pos.x = 0;
		pos.y = 0;
		pos.w = map.level[level]->w;
		pos.h = map.level[level]->h - cursor.speed_y;
		SDL_BlitSurface(map.transfer, NULL, map.level[level], &pos);
	  }
	} else {
	  // scroll the virtual screens up
	  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
		if(SDL_LockSurface(map.level[level]) == -1) {
		  fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		  fflush(stderr);
		  exit(0);	
		}
		skipped = (long)map.level[level]->pitch * (long)cursor.speed_y;
		memmove((Uint8*)(map.level[level]->pixels), 
				(Uint8*)((Uint8*)(map.level[level]->pixels) + skipped),
				(long)(map.level[level]->pitch * map.level[level]->h - skipped));
		SDL_UnlockSurface(map.level[level]);
	  }
	}

	// draw only the new left edge
	drawMapBottomEdge();

	break;
  }
}

int moveLeft(int checkCollision) {
  int move, old_pixel, old_pos, old_speed;

  old_speed = cursor.speed_x;
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
	  scrollMap(DIR_LEFT);	
	  if(map.accelerate) {
		if(cursor.speed_x < TILE_W) {
		  cursor.speed_x += SPEED_INC_X;
		  if(cursor.speed_x >= TILE_W) cursor.speed_x = TILE_W;
		}
	  }
	  return 1;
	}
	cursor.pixel_x = old_pixel;
	cursor.pos_x = old_pos;	
	if(!cursor.pixel_x) break;
	cursor.speed_x = cursor.pixel_x;
  }
  cursor.speed_x = old_speed;
  return 0;
}

int moveRight(int checkCollision) {
  int move, old_pixel, old_pos, old_speed;

  old_speed = cursor.speed_x;
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
	  scrollMap(DIR_RIGHT);	
	  if(map.accelerate) {
		if(cursor.speed_x < TILE_W) {
		  cursor.speed_x += SPEED_INC_X;
		  if(cursor.speed_x >= TILE_W) cursor.speed_x = TILE_W;
		}
	  }
	  return 1;
	}
	cursor.pixel_x = old_pixel;
	cursor.pos_x = old_pos;
	if(!cursor.pixel_x) break;
	cursor.speed_x = TILE_W - cursor.pixel_x;
  }
  cursor.speed_x = old_speed;
  return 0;
}

int moveUp(int checkCollision) {
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
	  scrollMap(DIR_UP);	
	  if(map.accelerate) {
		if(cursor.speed_y < TILE_H) {
		  cursor.speed_y += SPEED_INC_Y;
		  if(cursor.speed_y >= TILE_H) cursor.speed_y = TILE_H;
		}
	  }
	  return 1;
	}
	cursor.pixel_y = old_pixel;
	cursor.pos_y = old_pos;
	cursor.speed_y -= SPEED_INC_Y;
  }
  cursor.speed_y = old_speed;
  return 0;
}

int moveDown(int checkCollision) {
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
	if(move && (!checkCollision ||
				((cursor.gravity || map.detectLadder()) && map.detectCollision(DIR_DOWN)))) {
	  scrollMap(DIR_DOWN);	
	  if(map.accelerate) {
		if(cursor.speed_y < TILE_H) {
		  cursor.speed_y += SPEED_INC_Y;
		  if(cursor.speed_y >= TILE_H) cursor.speed_y = TILE_H;
		}
	  }
	  return 1;
	}
	cursor.pixel_y = old_pixel;
	cursor.pos_y = old_pos;
	cursor.speed_y -= SPEED_INC_Y;
  }
  cursor.speed_y = old_speed;
  return 0;
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
  if(dir == DIR_LEFT) {
	moveLeft(0);
  } else {
	moveRight(0);
  }
  
  // take a step up
  cursor.speed_y = (cursor.pixel_y == 0 ? TILE_H : cursor.pixel_y);
  cursor.stepup = 1;
  if(!moveUp(1)) {
	cursor.stepup = 0;
	// if can't step up
	cursor.speed_x = TILE_W;
	if(dir == DIR_LEFT) {
	  moveRight(0);
	} else {
	  moveLeft(0);
	}
	cursor.speed_x = 0;
	return 0;
  } else {
	cursor.stepup = 0;
	return 1;
  }
}

int moveJump() {
  int ret = 0;
  int old_speed;

  if(cursor.jump > 0) {
	cursor.jump--;

	old_speed = cursor.speed_y;
	cursor.speed_y = JUMP_SPEED;
	moveUp(1);
	cursor.speed_y = old_speed;
	ret = 1;
  }
  return ret;
}

/**
   Drop down.
 */
void moveGravity() {
  int old_speed;

  if(map.gravity) {
	if(map.detectLadder()) return;
	old_speed = cursor.speed_y;
	cursor.speed_y = 10;
	cursor.gravity = 1;
	moveDown(1);
	cursor.gravity = 0;
	cursor.speed_y = old_speed;
  }
}

int moveWithPlatform() {
  int old_speed, old_dir, diff;
  if(!cursor.platform) return 0;
  
  // move up or down if needed.
  old_speed = cursor.speed_y;
  old_dir = cursor.dir;
  diff = (cursor.platform->pos_y * TILE_H + cursor.platform->pixel_y) - 
	(cursor.pos_y * TILE_H + tom[0]->h + cursor.pixel_y);
  if(diff != 0) {
	if(cursor.platform->pos_y == cursor.pos_y + (tom[0]->h / TILE_H)) {
	  cursor.speed_y = abs(diff);
	  cursor.dir = DIR_UP;
	  moveUp(0);
	} else {
	  cursor.speed_y = abs(diff);
	  cursor.dir = DIR_DOWN;
	  moveDown(0);
	}
	fflush(stderr);
  }
  cursor.speed_y = old_speed;
  cursor.dir = old_dir;
  
  // move with the platform
  old_speed = cursor.speed_x;
  cursor.speed_x = cursor.platform->speed_x;
  if(cursor.platform->dir == DIR_LEFT) moveLeft(1);
  else moveRight(1);
  cursor.speed_x = old_speed;
  return 1;
}

/**
   The main thread loop
 */
void moveMap() {
  SDL_Event event;
  int delay;

  while(1) {

	// where is the map?
	map.top_left_x = cursor.pos_x - screen_center_x;
	map.top_left_y = cursor.pos_y - screen_center_y;

	// handle events.
	while(SDL_PollEvent(&event)) {
	  map.handleMapEvent(&event);
	}
	// jumping & platform
	if(!moveJump() && !moveWithPlatform()) {
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
	  if(!moveLeft(1)) {
		// try to step up onto the obsticle
		canStepUp(DIR_LEFT);
	  }
	  break;	
	case DIR_RIGHT:
	  if(!moveRight(1)) {
		// try to step up onto the obsticle
		canStepUp(DIR_RIGHT);
	  }
	  break;	
	case DIR_UP:
	  moveUp(1);
	  break;
	case DIR_DOWN:
	  moveDown(1);
	  break;
	case DIR_UPDATE:
	  cursor.dir = DIR_NONE;
	  break;
	}

	// check for monsters, etc.
	if(map.checkPosition) map.checkPosition();

	// update the screen
	finishDrawMap();

	// update the map?
	if(map.redraw) {
	  map.redraw = 0;
	  drawMap();
	}

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
  MapDrawParams params;
  SDL_Rect pos;
  int level;

  getMapDrawParams(&params);

  // draw on screen
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	pos.x = -(EXTRA_X * TILE_W);
	pos.y = -(EXTRA_Y * TILE_H);
	pos.w = map.level[level]->w;
	pos.h = map.level[level]->h;
	SDL_BlitSurface(map.level[level], NULL, screen, &pos);
	// make a callback if it exists
	if(level == LEVEL_MAIN) {	  
	  // draw Tom
	  if(map.afterMainLevelDrawn) map.afterMainLevelDrawn();
	  // draw creatures
	  if(map.monsters) drawLiveMonsters(screen, 
										(params.start_x - params.offset_x) * TILE_W + cursor.pixel_x,
										(params.start_y - params.offset_y) * TILE_H + cursor.pixel_y);
	}
  }

  // if the callback function is set, call it now.
  if(map.beforeDrawToScreen) {
	map.beforeDrawToScreen();
  }
    
  SDL_Flip(screen);
}

void setImage(int level, int index) {
  Position pos;
  pos.pos_x = cursor.pos_x;
  pos.pos_y = cursor.pos_y;
  setImagePosition(level, index, &pos);
  cursor.pos_x = pos.pos_x;
  cursor.pos_y = pos.pos_y;
  drawMap();
}

void setImagePosition(int level, int index, Position *pos) {
  int x, y, n;
  SDL_Rect rect, img_rect;
  int start_x, start_y, end_x, end_y;

  // clear the area 
  if(index != EMPTY_MAP) {
	img_rect.x = pos->pos_x;
	img_rect.y = pos->pos_y;
	img_rect.w = images[index]->image->w / TILE_W;
	img_rect.h = images[index]->image->h / TILE_H;
  }
  start_x = pos->pos_x - EXTRA_X;
  if(start_x < 0) start_x = 0;
  start_y = pos->pos_y - EXTRA_Y;
  if(start_y < 0) start_y = 0;
  end_x = pos->pos_x + (index != EMPTY_MAP ? images[index]->image->w / TILE_W : 1);
  if(end_x >= map.w) end_x = map.w;
  end_y = pos->pos_y + (index != EMPTY_MAP ? images[index]->image->h / TILE_H : 1);
  if(end_y >= map.h) end_y = map.h;
  for(y = start_y; y < end_y; y++) {
	for(x = start_x; x < end_x; x++) {
	  n = map.image_index[level][x + (y * map.w)];
	  if(n != EMPTY_MAP) {
		rect.x = x;
		rect.y = y;
		rect.w = images[n]->image->w / TILE_W;
		rect.h = images[n]->image->h / TILE_H;
		if(contains(&rect, pos->pos_x, pos->pos_y) || 
		   (index != EMPTY_MAP && intersects(&rect, &img_rect))) {	  
		  map.image_index[level][x + (y * map.w)] = EMPTY_MAP;
		}
	  }
	}
  }
  // add the image
  map.image_index[level][pos->pos_x + (pos->pos_y * map.w)] = index;
  // move the cursor
  if(index != EMPTY_MAP) {
	if(pos->pos_x + (images[index]->image->w / TILE_W) < map.w) 
	  pos->pos_x += (images[index]->image->w / TILE_W);
  }
  drawMap();
}

void setImageNoCheck(int level, int x, int y, int image_index) {
  map.image_index[level][x + (y * map.w)] = image_index;
}

int defaultDetectCollision(int dir) {
  return 1;
}

int defaultDetectLadder() {
  return 1;
}

void resetMap() {
  int i;

  map.gravity = 0;
  map.accelerate = 0;
  map.monsters = 0;
  map.beforeDrawToScreen = NULL;
  map.afterMainLevelDrawn = NULL;
  map.detectCollision = defaultDetectCollision;
  map.detectLadder = defaultDetectLadder;
  map.handleMapEvent = NULL;
  map.delay = 25;
  map.redraw = 0;

  // clean map
  for(i = 0; i < (map.w * map.h); i++) {
	map.image_index[LEVEL_BACK][i] = EMPTY_MAP;
	map.image_index[LEVEL_MAIN][i] = EMPTY_MAP;
	map.image_index[LEVEL_FORE][i] = EMPTY_MAP;
  }
}

int initMap(char *name, int w, int h) {
  int i;
  int hw_surface;

  // start a new Map
  map.gravity = 0;
  map.accelerate = 0;
  map.monsters = 0;
  map.name = strdup(name);
  map.w = w;
  map.h = h;
  map.beforeDrawToScreen = NULL;
  map.afterMainLevelDrawn = NULL;
  map.detectCollision = defaultDetectCollision;
  map.detectLadder = defaultDetectLadder;
  map.handleMapEvent = NULL;
  map.checkPosition = NULL;
  map.delay = 25;
  map.redraw = 0;
  for(i = LEVEL_BACK; i < LEVEL_COUNT; i++) {
	if(!(map.image_index[i] = (Uint16*)malloc(sizeof(Uint16) * w * h))) {
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
	hw_surface = (map.level[i]->flags & SDL_HWSURFACE ? 1 : 0);
	fprintf(stderr, "level[%d] is HW surface? %d\n", i, hw_surface);
	if(screen->flags & SDL_HWSURFACE & !hw_surface) {
	  fprintf(stderr, "Can't create surface in video memory. Since the screen is there, this surface must too.\n");
	  fflush(stderr);
	  return 0;
	}

	// set black as the transparent color key
	if(i > LEVEL_BACK) {
	  SDL_SetColorKey(map.level[i], SDL_SRCCOLORKEY, SDL_MapRGBA(map.level[i]->format, 0x00, 0x00, 0x00, 0xff));
	}
  }

  // init the transfer area
  if(!(map.transfer = SDL_CreateRGBSurface(SDL_HWSURFACE, 
										   screen->w + EXTRA_X * TILE_W, 
										   screen->h + EXTRA_Y * TILE_H, 
										   screen->format->BitsPerPixel, 
										   0, 0, 0, 0))) {
	fprintf(stderr, "Error creating surface: %s\n", SDL_GetError());
	fflush(stderr);
	return;
  }
  fprintf(stderr, "transfer area is HW surface? %d\n", hw_surface);
  if(screen->flags & SDL_HWSURFACE & !hw_surface) {
	fprintf(stderr, "Can't create surface in video memory. Since the screen is there, this surface must too.\n");
	fflush(stderr);
	return 0;
  }
  
  // clean map
  for(i = 0; i < (w * h); i++) {
	map.image_index[LEVEL_BACK][i] = EMPTY_MAP;
	map.image_index[LEVEL_MAIN][i] = EMPTY_MAP;
	map.image_index[LEVEL_FORE][i] = EMPTY_MAP;
  }

  // init some variables
  screen_center_x = (screen->w / TILE_W) / 2;
  screen_center_y = (screen->h / TILE_H) / 2;
  screen_w = screen->w / TILE_W;
  //  screen_h = (screen->h - edit_panel.image->h) / TILE_H;
  screen_h = screen->h / TILE_H;
  return 1;
}

void destroyMap() {
  int i;

  free(map.name);
  for(i = LEVEL_BACK; i < LEVEL_COUNT; i++) {
	SDL_FreeSurface(map.level[i]);
	free(map.image_index[i]);
  }
}

void resetCursor() {
  repositionCursor(0, 0);
}

void repositionCursor(int tile_x, int tile_y) {
  cursor.pos_x = tile_x;
  cursor.pos_y = tile_y;
  cursor.pixel_x = 0;
  cursor.pixel_y = 0;
  cursor.speed_x = TILE_W;
  cursor.speed_y = TILE_H;
  cursor.dir = DIR_NONE;
  cursor.wait = 0;
  cursor.jump = 0;
  cursor.gravity = 0;
  cursor.stepup = 0;
  cursor.platform = NULL;
}

void startJump() {
  startJumpN(JUMP_LENGTH);
}

void startJumpN(int n) {
  Position pos;
  pos.pos_x = cursor.pos_x;
  pos.pos_y = cursor.pos_y + tom[0]->h / TILE_H;
  pos.pixel_x = cursor.pixel_x;
  pos.pixel_y = cursor.pixel_y;
  pos.w = tom[0]->w / TILE_W;
  pos.h = 1;
  if(!cursor.jump && (containsType(&pos, TYPE_WALL) || cursor.platform)) {
	cursor.jump = n;
  }
}

void getGameCollisionCheck(GameCollisionCheck *check, Position *p) {
  check->start_x = p->pos_x - EXTRA_X;
  if(check->start_x < 0) check->start_x = 0;
  check->start_y = p->pos_y - EXTRA_Y;
  if(check->start_y < 0) check->start_y = 0;
  check->end_x = p->pos_x + p->w + (p->pixel_x > 0 ? 1 : 0);
  if(check->end_x >= map.w) check->end_x = map.w;
  check->end_y = p->pos_y + p->h + (p->pixel_y > 0 ? 1 : 0);
  if(check->end_y >= map.h) check->end_y = map.h;
  // tom's rect
  // FIXME: known issue, doesn't work near map's edge.
  check->rect.x = check->start_x + EXTRA_X;
  check->rect.y = check->start_y + EXTRA_Y;
  //  check->rect.w = tom[0]->w / TILE_W + (p->pixel_x > 0 ? 1 : 0);
  //  check->rect.h = tom[0]->h / TILE_H + (p->pixel_y > 0 ? 1 : 0);
  check->rect.w = p->w + (p->pixel_x > 0 ? 1 : 0);
  check->rect.h = p->h + (p->pixel_y > 0 ? 1 : 0);
}

/**
   Does the position contain a tile of this type?
   Return 1 if it does, 0 otherwise.
 */
int containsType(Position *p, int type) {
  return containsTypeWhere(p, NULL, type);
}

int containsTypeWhere(Position *p, Position *ret, int type) {
  GameCollisionCheck check;
  SDL_Rect rect, pos;
  int x, y, n;
  
  getGameCollisionCheck(&check, p);

  for(y = check.start_y; y < check.end_y; y++) {
	for(x = check.start_x; x < check.end_x;) {
	  n = map.image_index[LEVEL_MAIN][x + (y * map.w)];
	  if(n != EMPTY_MAP) {
		if(type & images[n]->type) {
		  rect.x = x;
		  rect.y = y;
		  rect.w = images[n]->image->w / TILE_W;
		  rect.h = images[n]->image->h / TILE_H;
		  if(intersects(&rect, &check.rect)) {
			if(ret) {
			  ret->pos_x = x;
			  ret->pos_y = y;
			  ret->pixel_x = 0;
			  ret->pixel_y = 0;
			  ret->w = rect.w;
			  ret->h = rect.h;
			}
			return 1;
		  }
		}
		x += images[n]->image->w / TILE_W;
	  } else {
		x++;
	  }
	}
  }
  return 0;
}

/**
   If the ground beneath pos is not COMPLETELY solid, this returns 0.
   1 otherwise.
 */
int onSolidGround(Position *p) {
  GameCollisionCheck check;
  SDL_Rect rect, pos;
  int x, y, n, r, found;
  
  getGameCollisionCheck(&check, p);

  //check.rect.h++;
  //if(check.rect.y + check.rect.h >= map.h) return 0;

  y = check.end_y;
  for(x = check.rect.x; x < check.end_x; x++) {
	found = 0;
	for(r = check.start_x; r <= x;) {
	  n = map.image_index[LEVEL_MAIN][r + (y * map.w)];
	  if(n != EMPTY_MAP) {
		rect.x = r;
		rect.y = y;
		rect.w = images[n]->image->w / TILE_W;
		rect.h = images[n]->image->h / TILE_H;
		if(contains(&rect, x, y)) {
		  //		if(intersects(&rect, &check.rect)) {
		  found = 1;
		  break;
		}
		r += images[n]->image->w / TILE_W;
	  } else {
		r++;
	  }
	}
	if(!found) return 0;
  }
  return 1;
}
