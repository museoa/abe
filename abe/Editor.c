#include "Editor.h"

SDL_Thread *thread;
int delay = 25;

void waitUntilPaintingStops();

/**
   Main editor event handling
*/
void editorMainLoop(SDL_Event *event) {
  switch(event->type) {
  case SDL_KEYDOWN:
	//	printf("The %s key was pressed! scan=%d\n", SDL_GetKeyName(event->key.keysym.sym), event->key.keysym.scancode);
	switch(event->key.keysym.sym) {
	case SDLK_LEFT: 
	  cursor.dir = DIR_LEFT; cursor.wait = 1; 
	  break;
	case SDLK_RIGHT: 
	  cursor.dir = DIR_RIGHT; cursor.wait = 1; 
	  break;
	case SDLK_UP: 
	  cursor.dir = DIR_UP; cursor.wait = 1; 
	  break;
	case SDLK_DOWN: 
	  cursor.dir = DIR_DOWN; cursor.wait = 1; 
	  break;
	case SDLK_RETURN: 
	  waitUntilPaintingStops();
	  setImage(edit_panel.level, edit_panel.image_index);
	  break;
	case SDLK_DELETE: case SDLK_BACKSPACE: 
	  waitUntilPaintingStops();
	  setImage(edit_panel.level, -1);
	  break;
	case SDLK_1: 
	  waitUntilPaintingStops();
	  edit_panel.level = LEVEL_BACK2;
	  drawMap();
	  break;
	case SDLK_2: 
	  waitUntilPaintingStops();
	  edit_panel.level = LEVEL_BACK1;
	  drawMap();
	  break;
	case SDLK_3: 
	  waitUntilPaintingStops();
	  edit_panel.level = LEVEL_MAIN;
	  drawMap();
	  break;
	case SDLK_4: 
	  waitUntilPaintingStops();
	  edit_panel.level = LEVEL_FORE;
	  drawMap();
	  break;
	case SDLK_5: 
	  waitUntilPaintingStops();
	  edit_panel.image_index--;
	  if(edit_panel.image_index < 0) {
		edit_panel.image_index = image_count - 1;
	  }
	  drawMap();
	  break;
	case SDLK_6: 
	  waitUntilPaintingStops();
	  edit_panel.image_index++;
	  if(edit_panel.image_index >= image_count) {
		edit_panel.image_index = 0;
	  }
	  drawMap();
	  break;
	}
	break;
  case SDL_KEYUP: 
	//	printf("The %s key was released! scan=%d\n", SDL_GetKeyName(event->key.keysym.sym), event->key.keysym.scancode);
	cursor.dir = DIR_NONE; break;
  }
}

void flipMap() {
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
  
  // draw the cursor
  int screen_center_x = (screen->w / TILE_W) / 2;
  int screen_center_y = (screen->h / TILE_H) / 2;  
  pos.x = screen_center_x * TILE_W;
  pos.y = screen_center_y * TILE_H;
  pos.w = TILE_W;
  pos.h = TILE_H;
  SDL_FillRect(screen, &pos, SDL_MapRGBA(screen->format, 0xa0, 0xa0, 0x00, 0x00));
  
  // draw the edit panel
  drawEditPanel();
  
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
  int screen_h = (screen->h - edit_panel.image->h) / TILE_H;
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
	  for(row = 0; row < map.level[level]->h - EDIT_PANEL_HEIGHT; row++) {
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
	flipMap();
  }
}

/**
   Move in the dir direction.
   This could be optimized by blitting the screen 1 tile unit in the 
   opposite direction and only drawing the new column.
 */
int moveMap(void *data) {
  while(1) {
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
	  SDL_Delay(delay);
	}
  }
}

/**
   A lame way to detect that the move thread has
   finished painting. FIXME.
 */
void waitUntilPaintingStops() {
  cursor.dir = DIR_NONE;
  SDL_Delay(delay + 10); // wait for the thread to stop painting
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

/**
   Draw the map where tile_x, tile_y is the center tile on the screen.
 */
void drawMap() {
  int screen_center_x = (screen->w / TILE_W) / 2;
  int screen_center_y = (screen->h / TILE_H) / 2;
  int screen_w = screen->w / TILE_W;
  int screen_h = (screen->h - edit_panel.image->h) / TILE_H;
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
  flipMap();
}

/*
void freeMap() {
  if(map) {
	// stop the thread
	cursor.dir = DIR_QUIT;
	SDL_WaitThread(thread, NULL);

	saveMap();
	free(map.name);
	int i;
	for(i = 0; i < LEVEL_COUNT; i++) {
	  free(map.image_index[i]);
	  SDL_FreeSurface(map.level[i]);
	}
	free(map);
  }  
}
*/

void drawEditPanel() {
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = edit_panel.image->w;
  rect.h = edit_panel.image->h;
  SDL_FillRect(edit_panel.image, &rect,  SDL_MapRGBA(screen->format, 0x2, 0x0, 0x0, 0x0));

  rect.x = 0;
  rect.y = 0;
  rect.w = edit_panel.image->w;
  rect.h = 5;
  SDL_FillRect(edit_panel.image, &rect,  SDL_MapRGBA(screen->format, 0x00, 0x00, 0x90, 0x00));


  // draw which level is being drawn
  int level;
  for(level = LEVEL_BACK2; level < LEVEL_COUNT; level++) {
	rect.x = 10 + (level * 40);
	rect.y = 10;
	rect.w = 20;
	rect.h = 20;
	SDL_FillRect(edit_panel.image, &rect, 
				 (level == edit_panel.level ? 
				  SDL_MapRGBA(screen->format, 0xff, 0x00, 0x00, 0x00) :
				  SDL_MapRGBA(screen->format, 0xff, 0xff, 0xff, 0x00)));
  }

  // draw which image is used
  rect.x = 200;
  rect.y = 10;
  rect.w = images[edit_panel.image_index]->image->w;
  rect.h = images[edit_panel.image_index]->image->h;
  SDL_BlitSurface(images[edit_panel.image_index]->image, NULL, 
				  edit_panel.image, &rect);

  // draw some instructions
  drawString(edit_panel.image, 10, 35, "change level 1234");
  drawString(edit_panel.image, 10, 35 + FONT_HEIGHT, "change image 56");
  drawString(edit_panel.image, 400, 5, "editor");
  drawString(edit_panel.image, 400, 5 + FONT_HEIGHT, "draw enter");
  drawString(edit_panel.image, 400, 5 + FONT_HEIGHT * 2, "erase del");
  

  // draw it on the screen
  rect.x = 0;
  rect.y = screen->h - edit_panel.image->h;
  rect.w = edit_panel.image->w;
  rect.h = edit_panel.image->h;
  SDL_BlitSurface(edit_panel.image, NULL, screen, &rect);
  //  SDL_Flip(screen);


  char s[80];
  sprintf(s, "x%d y%d level%d", cursor.pos_x, cursor.pos_y, edit_panel.level);
  drawString(screen, 5, 5, s);
}

void resetCursor() {
  cursor.pos_x = 0;
  cursor.pos_y = 0;
  cursor.dir = DIR_NONE;
  cursor.wait = 0;
}

void resetEditPanel() {
  edit_panel.level = LEVEL_MAIN;
  edit_panel.image_index = 0;
}

void allocMap(char *name, int w, int h) {
  // start a new Map
  map.name = strdup(name);
  map.w = w;
  map.h = h;
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

  // fill the map
  // step 1, fill up map with -1-s.
  int level;
  for(level = LEVEL_BACK2; level < LEVEL_COUNT; level++) {
	for(i = 0; i < map.h * map.w; i++) {
	  map.image_index[level][i] = -1;
	}
  }

  // step 2, fill the LEVEL_BACK2 with background tiles.
  int x, y;
  Image *img = images[img_back];
  int step_x = img->image->w / TILE_W;
  int step_y = img->image->h / TILE_H;
  for(y = 0; y < map.h; y += step_y) {
	for(x = 0; x < map.w; x += step_x) {
	  i = x + (y * map.w);
	  map.image_index[LEVEL_BACK2][i] = img_back;
	}
  }

  // step 3, fill the LEVEL_MAIN with rock tiles.
  img = images[img_rock];
  step_x = img->image->w / TILE_W;
  step_y = img->image->h / TILE_H;
  int r;
  for(y = 0; y < map.h; y += step_y) {
	for(x = 0; x < map.w; ) {
	  r = (int)(5.0 * rand() / (RAND_MAX + 1.0));
	  if(r) {
		x++;
	  } else {
		i = x + (y * map.w);
		map.image_index[LEVEL_MAIN][i] = img_rock;
		x += step_x;
	  }
	}
  }
  
  // reset the cursor
  resetCursor();

  // initialize the edit panel
  resetEditPanel();
  
  // start the thread
  if(!(thread = SDL_CreateThread(moveMap, NULL))) {
	fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
	fflush(stderr);
	exit(0);
  }
}

void initEditor() {
  //  map = NULL;
  if(!(edit_panel.image = SDL_CreateRGBSurface(SDL_HWSURFACE, 
											   screen->w, EDIT_PANEL_HEIGHT, 
											   screen->format->BitsPerPixel, 
											   0, 0, 0, 0))) {
	fprintf(stderr, "Error creating surface: %s\n", SDL_GetError());
	fflush(stderr);
	return;
  }
}

void editMap(char *name, int w, int h) {
  // save the current map
  // freeMap();

  // create a new one
  allocMap(name, w, h);

  // show it
  drawMap();
}

void saveMap() {
  printf("IMPLEMENT ME: saveMap().\n");
  fflush(stdout);
}

void loadMap(char *name) {
  printf("IMPLEMENT ME: loadMap(). name=%s\n", name);
  fflush(stdout);
}
