#include "Map.h"

void waitUntilPaintingStops();
void finishDrawMap();

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

	for(y = params.start_y; y < params.end_y; y++) {
	  for(x = params.start_x; x < params.end_x;) {
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  pos.x = (params.offset_x + (x - params.start_x)) * TILE_W;
		  pos.y = (params.offset_y + (y - params.start_y)) * TILE_H;
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  pos.x += EXTRA_X * TILE_W;
		  pos.y += EXTRA_Y * TILE_H;		  
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
  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the left edge param
  params.start_x = (cursor.pos_x - screen_center_x) - EXTRA_X;

  SDL_Rect pos;
  int n, row, level, x, y;
  // erase the edge
  pos.x = 0;
  pos.y = 0;
  pos.w = TILE_W;
  pos.h = map.level[0]->h;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }
   
  if(params.start_x >= 0) {
	// redraw the left edge of the screen
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  for(y = params.start_y; y < params.end_y; y++) {
		x = params.start_x;
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  //pos.x = (offset_x + (x - start_x)) * TILE_W;
		  pos.x = 0;
		  pos.y = (params.offset_y + (y - params.start_y)) * TILE_H;
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  //pos.x += EXTRA_X * TILE_W;
		  pos.y += EXTRA_Y * TILE_H;		  
		  SDL_BlitSurface(images[n]->image, NULL, map.level[level], &pos);
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
  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the top edge param
  params.start_y = (cursor.pos_y - screen_center_y) - EXTRA_Y;

  SDL_Rect pos;
  int n, row, level, x, y;
  // erase the edge
  pos.x = 0;
  pos.y = 0;
  pos.w = map.level[0]->w;
  pos.h = TILE_W;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }
   
  if(params.start_y >= 0) {
	// redraw the left edge of the screen
	for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	  y = params.start_y;
	  for(x = params.start_x; x < params.end_x;) {
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  pos.x = (params.offset_x + (x - params.start_x)) * TILE_W;
		  pos.y = 0;
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  pos.x += EXTRA_X * TILE_W;
		  //pos.y += EXTRA_Y * TILE_H;		  
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
void drawMapRightEdge() {
  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the right edge param
  params.end_x = (cursor.pos_x - screen_center_x) + screen_w;

  SDL_Rect pos;
  int n, row, level, x, y;
  // erase the edge
  pos.x = map.level[0]->w - TILE_W;
  pos.y = 0;
  pos.w = TILE_W;
  pos.h = map.level[0]->h;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }
   
  // redraw the left edge of the screen
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	for(y = params.start_y; y < params.end_y; y++) {
	  // here we have to draw more than 1 column b/c images
	  // extend from right_edge-EXTRA_X on. 
	  for(x = params.end_x - EXTRA_X; x < params.end_x;) {
		if(params.end_x >= map.w) break;
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  //pos.x = (offset_x + (x - start_x)) * TILE_W;
		  pos.x = map.level[level]->w - ((params.end_x - x) * TILE_W);
		  pos.y = (params.offset_y + (y - params.start_y)) * TILE_H;
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  //pos.x += EXTRA_X * TILE_W;
		  pos.y += EXTRA_Y * TILE_H;
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
  // compute what to draw
  MapDrawParams params;
  getMapDrawParams(&params);

  // override the bottom edge param
  params.end_y = (cursor.pos_y - screen_center_y) + screen_h;

  SDL_Rect pos;
  int n, row, level, x, y;
  // erase the edge
  pos.x = 0;
  pos.y = map.level[0]->h - TILE_H;
  pos.w = map.level[0]->w;
  pos.h = TILE_H;
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	if(level == LEVEL_BACK) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	}
  }
   
  // redraw the left edge of the screen
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
	// here we have to draw more than 1 column b/c images
	// extend from right_edge-EXTRA_X on. 
	for(y = params.end_y - EXTRA_Y; y < params.end_y; y++) {
	  if(params.end_y >= map.h) break;
	  for(x = params.start_x; x < params.end_x;) {
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  pos.x = (params.offset_x + (x - params.start_x)) * TILE_W;
		  pos.y = map.level[level]->h - ((params.end_y - y) * TILE_H);
		  pos.w = images[n]->image->w;
		  pos.h = images[n]->image->h;
		  
		  // compensate for extra area
		  pos.x += EXTRA_X * TILE_W;
		  //pos.y += EXTRA_Y * TILE_H;
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

void scrollMap(int dir) {
  if(dir == DIR_NONE) return;

  int not_implemented = 0;

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
		memmove((Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row) + (long)TILE_W), 
				(Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row)),
				(long)(map.level[level]->w - TILE_W) * (long)sizeof(Uint16));
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
				(Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row + (long)TILE_W)),
				(long)(map.level[level]->w - TILE_W) * (long)sizeof(Uint16));
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
	  long skipped = (long)map.level[level]->w * (long)TILE_H;
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
	  long skipped = (long)map.level[level]->w * (long)TILE_H;
	  memmove((Uint16*)(map.level[level]->pixels), 
			  (Uint16*)((Uint16*)(map.level[level]->pixels) + skipped),
			  (long)(map.level[level]->w * map.level[level]->h - skipped) * (long)sizeof(Uint16));
	  SDL_UnlockSurface(map.level[level]);
	}

	// draw only the new left edge
	drawMapBottomEdge();

	break;


  default:
	// by default just draw the old way
	not_implemented = 1;
  }

  if(not_implemented) {
	drawMap();
  } else {
	finishDrawMap();
  }
}

/**
   Move in the dir direction.
   This could be optimized by blitting the screen 1 tile unit in the 
   opposite direction and only drawing the new column.
 */
int moveMap(void *data) {
  while(!map.stopThread) {
	switch(cursor.dir) {
	case DIR_QUIT:
	  return 0;
	case DIR_LEFT:
	  cursor.pos_x--;
	  if(cursor.pos_x < 0) {
		cursor.pos_x = 0;
		cursor.dir = DIR_NONE;
	  } else {
		scrollMap(DIR_LEFT);
	  }
	  break;	
	case DIR_RIGHT:
	  cursor.pos_x++;
	  if(cursor.pos_x >= map.w) {
		cursor.pos_x = map.w - 1;
		cursor.dir = DIR_NONE;
	  } else {
		scrollMap(DIR_RIGHT);
	  }
	  break;
	case DIR_UP:
	  cursor.pos_y--;
	  if(cursor.pos_y < 0) {
		cursor.pos_y = 0;
		cursor.dir = DIR_NONE;
	  } else {
		scrollMap(DIR_UP);
	  }
	  break;
	case DIR_DOWN:
	  cursor.pos_y++;
	  if(cursor.pos_y >= map.h) {
		cursor.pos_y = map.h - 1;
		cursor.dir = DIR_NONE;
	  } else {
		scrollMap(DIR_DOWN);
	  }
	  break;
	case DIR_NONE:
	  // wait until there's movement.
	  if(SDL_mutexP(map.move_cond_mutex) == -1){
		fprintf(stderr, "Couldn't lock mutex\n");
		fflush(stderr);
		exit(-1);
	  }
	  if(SDL_CondWait(map.move_cond, map.move_cond_mutex) == -1) {
		fprintf(stderr, "Couldn't wait on condition\n");
		fflush(stderr);
		exit(-1);
	  }
	}
	if(cursor.wait) {
	  cursor.wait = 0;
	  SDL_Delay(map.delay);
	} else {
	  SDL_Delay(10); // a small delay to not max out the cpu
	}
  }
}

/**
   A lame way to detect that the move thread has
   finished painting. FIXME.
 */
void waitUntilPaintingStops() {
  cursor.dir = DIR_NONE;
  SDL_Delay(map.delay + 10); // wait for the thread to stop painting
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

void initMap(char *name, int w, int h) {
  // start a new Map
  map.name = strdup(name);
  map.w = w;
  map.h = h;
  map.beforeDrawToScreen = NULL;
  map.delay = 25;
  map.thread = NULL;
  map.move_cond = NULL;
  map.move_cond_mutex = NULL;
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
  stopMapMoveThread();
  free(map.name);
  int i;
  for(i = LEVEL_BACK; i < LEVEL_COUNT; i++) {
	SDL_FreeSurface(map.level[i]);
	free(map.image_index[i]);
  }
}

void startMapMoveThread() {
  // Create the cond
  if(!map.thread) {
	if(!(map.move_cond = SDL_CreateCond())) {
	  fprintf(stderr, "Unable to create condition variable: %s\n", SDL_GetError());
	  fflush(stderr);
	  exit(0);
	}
	// Create the mutex
	if(!(map.move_cond_mutex = SDL_CreateMutex())) {
	  fprintf(stderr, "Unable to create mutex: %s\n", SDL_GetError());
	  fflush(stderr);
	  exit(0);
	}
	// start the thread
	map.stopThread = 0;
	if(!(map.thread = SDL_CreateThread(moveMap, NULL))) {
	  fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
	  fflush(stderr);
	  exit(0);
	}
  }
}

void stopMapMoveThread() {
  if(map.thread) {
	map.stopThread == 1;
	SDL_WaitThread(map.thread, NULL);
	// kill the condition var
	SDL_DestroyCond(map.move_cond);
	// kill the mutex
	SDL_DestroyMutex(map.move_cond_mutex);
	map.thread = NULL;
	map.move_cond = NULL;
	map.move_cond_mutex = NULL;
  }
}

/**
   You must call this method if previously the cursor's direction was DIR_NONE.
   This wakes up the movement thread.
 */
void signalMapMoveThread() {
  if(SDL_CondSignal(map.move_cond) == -1) {
	fprintf(stderr, "Couldn't signal on condition\n");
	fflush(stderr);
	exit(-1);
  }
}

void resetCursor() {
  cursor.pos_x = 0;
  cursor.pos_y = 0;
  cursor.dir = DIR_NONE;
  cursor.wait = 0;
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
  fwrite(&(map.w), sizeof(map.w), 1, fp);
  fwrite(&(map.h), sizeof(map.h), 1, fp);
  // FIXME: use runtime compression
  int i;
  for(i = 0; i < LEVEL_COUNT; i++) {
	fwrite(map.image_index[i], sizeof(int) * map.w * map.h, 1, fp);
  }
  fclose(fp);
}

// call this after initMap()!
void loadMap() {
  char path[300];
  sprintf(path, "%s/%s.dat", MAPS_DIR, map.name);
  printf("Loading map %s\n", path);  
  fflush(stdout);
  FILE *fp;
  if(!(fp = fopen(path, "rb"))) {
	fprintf(stderr, "Can't open file for reading.");
	fflush(stderr);
	return;
  }
  fread(&(map.w), sizeof(map.w), 1, fp);
  fread(&(map.h), sizeof(map.h), 1, fp);
  // FIXME: use runtime compression
  int i;
  for(i = 0; i < LEVEL_COUNT; i++) {
	fread(map.image_index[i], sizeof(int) * map.w * map.h, 1, fp);
  }
  fclose(fp);

  resetCursor();
  drawMap();
}
