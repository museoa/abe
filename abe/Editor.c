#include "Editor.h"

/**
   The editor-specific map drawing event.
   This is called before the map is sent to the screen.
 */
void beforeDrawToScreen() {
  SDL_Rect pos;

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
}

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
	  edit_panel.level = LEVEL_BACK;
	  drawMap();
	  break;
	case SDLK_2: 
	  waitUntilPaintingStops();
	  edit_panel.level = LEVEL_MAIN;
	  drawMap();
	  break;
	case SDLK_3: 
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
  for(level = LEVEL_BACK; level < LEVEL_COUNT; level++) {
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
  drawString(edit_panel.image, 10, 35, "change level 123");
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
  initMap(name, w, h);
  
  // set our painting events
  map.beforeDrawToScreen = beforeDrawToScreen;

  // fill the map
  int level, i, x, y;


  // add some tiles in the background
  Image *img = images[img_back];
  int step_x = img->image->w / TILE_W;
  int step_y = img->image->h / TILE_H; 
  for(y = 0; y < map.h; y += step_y) {
	for(x = 0; x < map.w; x += step_x) {
	  i = x + (y * map.w);
	  map.image_index[LEVEL_BACK][i] = img_back;
	}
  }

  // fill the LEVEL_MAIN with rock tiles.
  img = images[img_rock];
  step_x = img->image->w / TILE_W;
  step_y = img->image->h / TILE_H;
  int r;
  for(y = 0; y < map.h; y += step_y) {
	for(x = 0; x < map.w; ) {
	  r = (int)(8.0 * rand() / (RAND_MAX + 1.0));
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

  startMapMoveThread();
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
