#include "Game.h"

typedef struct _gameCollisionCheck {
  int start_x, start_y, end_x, end_y;
  SDL_Rect rect;
} GameCollisionCheck;

/**
   The editor-specific map drawing event.
   This is called before the map is sent to the screen.
 */
void afterMainLevelDrawn() {
  SDL_Rect pos;

  // draw Tom
  int screen_center_x = (screen->w / TILE_W) / 2;
  int screen_center_y = (screen->h / TILE_H) / 2;  
  pos.x = screen_center_x * TILE_W;
  pos.y = screen_center_y * TILE_H;
  pos.w = tom[0]->w;
  pos.h = tom[0]->h;
  game.face++;
  if(game.face >= FACE_COUNT) game.face = 0;  
  if(cursor.dir == DIR_UPDATE || cursor.dir == DIR_NONE) game.face = FACE_STILL;
  SDL_BlitSurface(tom[(game.dir == GAME_DIR_LEFT ? game.face : game.face + FACE_COUNT)], 
				  NULL, screen, &pos);
}

void gameBeforeDrawToScreen() {
  char s[80];
  sprintf(s, "x%d y%d", cursor.pos_x, cursor.pos_y);
  drawString(screen, 5, 5, s);
  sprintf(s, "px%d py%d", cursor.pixel_x, cursor.pixel_y);
  drawString(screen, 5, 5 + FONT_HEIGHT, s);
  sprintf(s, "spx%d spy%d", cursor.speed_x, cursor.speed_y);
  drawString(screen, 5, 5 + FONT_HEIGHT * 2, s);
}

/**
   Main game event handling
*/
void gameMainLoop(SDL_Event *event) {
  switch(event->type) {
  case SDL_KEYDOWN:
	//	printf("The %s key was pressed! scan=%d\n", SDL_GetKeyName(event->key.keysym.sym), event->key.keysym.scancode);
	switch(event->key.keysym.sym) {
	case SDLK_LEFT: 
	  cursor.dontMove = (cursor.dir == DIR_RIGHT ? 1 : 0);
	  game.dir = GAME_DIR_LEFT;
	  cursor.dir = DIR_LEFT;
	  signalMapMoveThread();
	  break;
	case SDLK_RIGHT: 
	  cursor.dontMove = (cursor.dir == DIR_LEFT ? 1 : 0);
	  game.dir = GAME_DIR_RIGHT;
	  cursor.dir = DIR_RIGHT;
	  signalMapMoveThread();
	  break;
	case SDLK_UP: 
	  cursor.dir = DIR_UP;
	  signalMapMoveThread();
	  break;
	case SDLK_DOWN: 
	  cursor.dir = DIR_DOWN; 
	  signalMapMoveThread();
	  break;
	case SDLK_SPACE: 
	  drawMap();
	  break;
	}
	break;
  case SDL_KEYUP: 
	cursor.dir = DIR_UPDATE; 
	signalMapMoveThread();
	break;
  }
}

/**
   When moving left/right we can step up onto max 1 tile height obsticles.
   This method checks and if possible, jumps up on it.
   It returns 1 on success and 0 otherwise.
 */
/*
int canStepUp(int pos_x, int pos_y, int original_dir) {
  // Can we step up?
  int start_y = pos_y - 1 - (tom[0]->h / TILE_H);
  if(start_y < 0) return 0;
  int x, y, n;
  for(y = start_y; y < start_y + (tom[0]->h / TILE_H) && y < map.h; y++) {
	for(x = pos_x; x < pos_x + (tom[0]->w / TILE_W) && x < map.w; x++) {
	  n = map.image_index[LEVEL_MAIN][x + (y * map.w)];
	  if(n > -1) {
		if(images[n]->type == TYPE_WALL) {
		  return 0;
		}
	  }
	}
  }
  // if yes, step up
  cursor.dir = DIR_UP;
  cursor.speed_y = TILE_H;
  moveUp(0);
  cursor.dir = original_dir;
  return 1;
}
*/

GameCollisionCheck getGameCollisionCheck() {
  GameCollisionCheck check;
  //  check.start_x = cursor.pos_x + (cursor.pixel_x > 0 ? 1 : 0) - EXTRA_X;
  check.start_x = cursor.pos_x - EXTRA_X;
  if(check.start_x < 0) check.start_x = 0;
  //  check.start_y = cursor.pos_y + (cursor.pixel_y > 0 ? 1 : 0) - EXTRA_Y;
  check.start_y = cursor.pos_y - EXTRA_Y;
  if(check.start_y < 0) check.start_y = 0;
  check.end_x = cursor.pos_x + (tom[0]->w / TILE_W) + (cursor.pixel_x > 0 ? 1 : 0);
  if(check.end_x >= map.w) check.end_x = map.w;
  check.end_y = cursor.pos_y + (tom[0]->h / TILE_H) + (cursor.pixel_y > 0 ? 1 : 0);
  if(check.end_y >= map.h) check.end_y = map.h;
  // tom's rect
  // FIXME: known issue, doesn't work near map's edge.
  check.rect.x = check.start_x + EXTRA_X;
  check.rect.y = check.start_y + EXTRA_Y;
  check.rect.w = tom[0]->w / TILE_W + (cursor.pixel_x > 0 ? 1 : 0);
  check.rect.h = tom[0]->h / TILE_H + (cursor.pixel_y > 0 ? 1 : 0);
  return check;
}

// return a 1 to proceed, 0 to stop
int detectCollision(int dir) {
  int x, y, n;
  GameCollisionCheck check = getGameCollisionCheck();
  int ret = 1;
  SDL_Rect rect;
  // are we in a wall?
  for(y = check.start_y; y < check.end_y; y++) {
	for(x = check.start_x; x < check.end_x;) {
	  n = map.image_index[LEVEL_MAIN][x + (y * map.w)];
	  if(n > -1) {
		if(images[n]->type == TYPE_WALL) {
		  rect.x = x;
		  rect.y = y;
		  rect.w = images[n]->image->w / TILE_W;
		  rect.h = images[n]->image->h / TILE_H;
		  if(intersects(&rect, &check.rect)) {
			ret = 0;
			break;
		  }
		}
		x += images[n]->image->w / TILE_W;
	  } else {
		x++;
	  }
	}
	if(!ret) break;
  }
  return ret;
}

void runMap(char *name, int w, int h) {
  initMap(name, w, h);
  // try to load the map and quit if you can't find it.
  if(!loadMap(0)) {
	fprintf(stderr, "Can't find map file: %s", name);
	fflush(stderr);
	exit(0);
  }
  cursor.pos_x = 30;
  cursor.pos_y = 16;
  cursor.speed_x = 8;
  cursor.speed_y = 8;
  drawMap();
  // set our painting events
  map.beforeDrawToScreen = gameBeforeDrawToScreen;
  map.afterMainLevelDrawn = afterMainLevelDrawn;
  map.detectCollision = detectCollision;
  // activate gravity and accelerated movement
  map.accelerate = 1;
  map.gravity = 1;
  // start the move thread
  startMapMoveThread();
}

void initGame() {
  game.face = 0;
  game.dir = GAME_DIR_RIGHT;
}
