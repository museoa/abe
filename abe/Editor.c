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
	  cursor.dir = DIR_LEFT;
	  break;
	case SDLK_RIGHT: 
	  cursor.dir = DIR_RIGHT;
	  break;
	case SDLK_UP: 
	  cursor.dir = DIR_UP;
	  break;
	case SDLK_DOWN: 
	  cursor.dir = DIR_DOWN;
	  break;
	case SDLK_PAGEDOWN:
	  cursor.pos_y += screen->h / TILE_H;
	  if(cursor.pos_y >= map.h) cursor.pos_y = map.h - 1;
	  cursor.dir = DIR_NONE;
	  map.redraw = 1;
	  break;
	case SDLK_PAGEUP:
	  cursor.pos_y -= screen->h / TILE_H;
	  if(cursor.pos_y < 0) cursor.pos_y = 0;
	  cursor.dir = DIR_NONE;
	  map.redraw = 1;
	  break;
	case SDLK_END:
	  cursor.pos_x += screen->w / TILE_W;
	  if(cursor.pos_x >= map.w) cursor.pos_x = map.w - 1;
	  cursor.dir = DIR_NONE;
	  map.redraw = 1;
	  break;
	case SDLK_HOME:
	  cursor.pos_x -= screen->w / TILE_H;
	  if(cursor.pos_x < 0) cursor.pos_x = 0;
	  cursor.dir = DIR_NONE;
	  map.redraw = 1;
	  break;
	case SDLK_RETURN: 
	  setImage(edit_panel.level, edit_panel.image_index);
	  break;
	case SDLK_DELETE: case SDLK_BACKSPACE: 
	  setImage(edit_panel.level, -1);
	  break;
	case SDLK_1: 
	  edit_panel.level = LEVEL_BACK;
 	  drawMap();
	  break;
	case SDLK_2: 
	  edit_panel.level = LEVEL_MAIN;
	  drawMap();
	  break;
	case SDLK_3: 
	  edit_panel.level = LEVEL_FORE;
	  drawMap();
	  break;
	case SDLK_5: 
	  edit_panel.image_index--;
	  if(edit_panel.image_index < 0) {
		edit_panel.image_index = image_count - 1;
	  }
	  drawMap();
	  break;
	case SDLK_6: 
	  edit_panel.image_index++;
	  if(edit_panel.image_index >= image_count) {
		edit_panel.image_index = 0;
	  }
	  drawMap();
	  break;
	case SDLK_l: 
	  loadMap(1);
	  break;
	case SDLK_s: 
	  saveMap();
	  break;
	case SDLK_ESCAPE: 
	  cursor.dir = DIR_QUIT;
	  break;
	}
	break;
  case SDL_KEYUP: 
	//printf("The %s key was released! scan=%d\n", SDL_GetKeyName(event->key.keysym.sym), event->key.keysym.scancode);
	cursor.dir = DIR_NONE; 
	break;
  }
}

void drawEditPanel() {
  SDL_Rect rect;
  int level;
  char s[80];

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


  sprintf(s, "x%d y%d level%d dir%d", cursor.pos_x, cursor.pos_y, edit_panel.level, cursor.dir);
  drawString(screen, 5, 5, s);
  sprintf(s, "spx%d spy%d px%d py%d", cursor.speed_x, cursor.speed_y, cursor.pixel_x, cursor.pixel_y);
  drawString(screen, 5, 5 + FONT_HEIGHT, s);
}

void resetEditPanel() {
  edit_panel.level = LEVEL_MAIN;
  edit_panel.image_index = 0;
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
  int level, i, x, y;
  int step_x, step_y;
  Image *img;

  resetMap();

  // set our painting events
  map.beforeDrawToScreen = beforeDrawToScreen;
  
  // our event handling
  map.handleMapEvent = editorMainLoop;
  
  // add some tiles in the background
  img = images[img_back];
  step_x = img->image->w / TILE_W;
  step_y = img->image->h / TILE_H; 
  for(y = 0; y < map.h; y += step_y) {
	for(x = 0; x < map.w; x += step_x) {
	  i = x + (y * map.w);
	  map.image_index[LEVEL_BACK][i] = img_back;
	  // make a border
	  if(y == 0 || y == map.h - 4 || x == 0 || x == map.w - 4) {
		map.image_index[LEVEL_MAIN][i] = img_rock;		
	  }
	}
  }
  
  // reset the cursor
  resetCursor();
  
  // initialize the edit panel
  resetEditPanel();
  
  // try to load an existing map
  loadMap(0);
  drawMap();

  // start the map's main loop
  moveMap();
}
