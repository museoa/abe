#include "Map.h"

void waitUntilPaintingStops();
void finishDrawMap();

/**
   Draw the map where tile_x, tile_y is the center tile on the screen.
 */
void drawMap() {
  int screen_center_x = (screen->w / TILE_W) / 2;
  int screen_center_y = (screen->h / TILE_H) / 2;
  int screen_w = screen->w / TILE_W;
  //  int screen_h = (screen->h - edit_panel.image->h) / TILE_H;
  int screen_h = screen->h / TILE_H;
  int start_x = cursor.pos_x - screen_center_x;
  int start_y = cursor.pos_y - screen_center_y;
  int end_x = start_x + screen_w; 
  int end_y = start_y + screen_h;

  int offset_x = 0, offset_y = 0;
  int n;
  if(start_x < 0) {
	offset_x = -start_x;
	start_x = 0;
  } else {
	n = (start_x >= EXTRA_X ? EXTRA_X : start_x);
	offset_x = -n;
	start_x -= n;	
  }
  if(start_y < 0) {
	offset_y = -start_y;
	start_y = 0;	
  } else {
	n = (start_y >= EXTRA_Y ? EXTRA_Y : start_y);
	offset_y = -n;
	start_y -= n;	
  }
  if(end_x > map.w) end_x = map.w;
  if(end_y > map.h) end_y = map.h;

  SDL_Rect pos;

  // draw the map
  int x, y, level;
  for(level = LEVEL_BACK2; level < LEVEL_COUNT; level++) {

	// erase the screen
	pos.x = 0;
	pos.y = 0;
	pos.w = map.level[level]->w;
	pos.h = map.level[level]->h;
	if(level == LEVEL_BACK2) {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	} else {
	  SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x0, 0x0, 0x0, 0x00));
	}

	for(y = start_y; y < end_y; y++) {
	  for(x = start_x; x < end_x;) {
		n = (map.image_index[level][x + (y * map.w)]);
		if(n > -1) {
		  // Draw the image
		  // should be some check here in case images[n] points to la-la-land.
		  pos.x = (offset_x + (x - start_x)) * TILE_W;
		  pos.y = (offset_y + (y - start_y)) * TILE_H;
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

void finishDrawMap() {
  // draw on screen
  SDL_Rect pos;
  int level;
  for(level = LEVEL_BACK2; level < LEVEL_COUNT; level++) {
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

void scrollMap(int dir) {
  if(dir == DIR_NONE) return;

  int not_implemented = 0;

  // move the screen
  int row, level, x, y;

  int screen_center_x = (screen->w / TILE_W) / 2;
  int screen_center_y = (screen->h / TILE_H) / 2;
  //int screen_w = screen->w / TILE_W;
  //  int screen_h = (screen->h - edit_panel.image->h) / TILE_H;
  int screen_h = screen->h / TILE_H;
  int start_x = cursor.pos_x - screen_center_x;
  int start_y = cursor.pos_y - screen_center_y;
  //int end_x = start_x + screen_w; 
  int end_y = start_y + screen_h;

  int offset_x = 0, offset_y = 0;
  int n;

  /*
  if(start_x < 0) {
	offset_x = -start_x;
	start_x = 0;
  } else {
	n = (start_x >= EXTRA_X ? EXTRA_X : start_x);
	offset_x = -n;
	start_x -= n;	
  }
  */
  if(start_y < 0) {
	offset_y = -start_y;
	start_y = 0;	
  } else {
	n = (start_y >= EXTRA_Y ? EXTRA_Y : start_y);
	offset_y = -n;
	start_y -= n;	
  }
  //if(end_x > map.w) end_x = map.w;
  if(end_y > map.h) end_y = map.h;
  
  SDL_Rect pos;
  switch(dir) {
  case DIR_LEFT:

	// scroll the virtual screens right row by row
	for(level = LEVEL_BACK2; level < LEVEL_COUNT; level++) {
	  if(SDL_LockSurface(map.level[level]) == -1) {
		fprintf(stderr, "Unable to lock surface for scrolling: %s\n", SDL_GetError());
		fflush(stderr);
		exit(0);	
	  }
	  //	  for(row = 0; row < map.level[level]->h - EDIT_PANEL_HEIGHT; row++) {
	  for(row = 0; row < map.level[level]->h; row++) {
		memmove((Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row) + (long)TILE_W), 
				(Uint16*)((Uint16*)(map.level[level]->pixels) + ((long)map.level[level]->w * (long)row)),
				(long)(map.level[level]->w - TILE_W) * (long)sizeof(Uint16));
	  }
	  SDL_UnlockSurface(map.level[level]);
	}

	// erase the edge
	pos.x = 0;
	pos.y = 0;
	pos.w = TILE_W;
	pos.h = map.level[0]->h;
	for(level = LEVEL_BACK2; level < LEVEL_COUNT; level++) {
	  if(level == LEVEL_BACK2) {
		SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x20, 0x20, 0x20, 0x00));
	  } else {
		SDL_FillRect(map.level[level], &pos, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0x00));
	  }
	}
	
	// adjust end_x
	start_x -= EXTRA_X;

	if(start_x >= 0) {
	  // redraw the left edge of the screen
	  for(level = LEVEL_BACK2; level < LEVEL_COUNT; level++) {
		for(y = start_y; y < end_y; y++) {
		  x = start_x;
		  n = (map.image_index[level][x + (y * map.w)]);
		  if(n > -1) {
			// Draw the image
			// should be some check here in case images[n] points to la-la-land.
			//pos.x = (offset_x + (x - start_x)) * TILE_W;
			pos.x = 0;
			pos.y = (offset_y + (y - start_y)) * TILE_H;
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
	}
	if(cursor.wait) {
	  cursor.wait = 0;
	  SDL_Delay(map.delay);
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
  int i;
  for(i = 0; i < LEVEL_COUNT; i++) {
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
	if(i > LEVEL_BACK2) {
	  SDL_SetColorKey(map.level[i], SDL_SRCCOLORKEY, SDL_MapRGBA(map.level[i]->format, 0x00, 0x00, 0x00, 0xff));
	}
  }
}

void startMapMoveThread() {
  // start the thread
  map.stopThread = 0;
  if(!(map.thread = SDL_CreateThread(moveMap, NULL))) {
	fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
	fflush(stderr);
	exit(0);
  }
}

void stopMapMoveThread() {
  if(map.thread) {
	map.stopThread == 1;
	SDL_WaitThread(map.thread, NULL);
  }
}

