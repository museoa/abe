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

  // change the face
  if(cursor.dir == DIR_LEFT || cursor.dir == DIR_RIGHT) {
	game.face++;
	if(game.face >= FACE_COUNT * FACE_STEP) game.face = 0;
  }
  SDL_BlitSurface(tom[(game.dir == GAME_DIR_LEFT ? 
					   (game.face / FACE_STEP) : 
					   (game.face / FACE_STEP) + FACE_COUNT)], 
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
  sprintf(s, "dir%d", cursor.dir);
  drawString(screen, 5, 5 + FONT_HEIGHT * 3, s);
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
	  game.dir = GAME_DIR_LEFT;
	  cursor.dir = DIR_LEFT;
 	  cursor.speed_x = START_SPEED_X;
	  break;
	case SDLK_RIGHT: 
	  game.dir = GAME_DIR_RIGHT;
	  cursor.dir = DIR_RIGHT;
 	  cursor.speed_x = START_SPEED_X;
	  break;
	case SDLK_UP: 
	  cursor.dir = DIR_UP;
	  cursor.speed_y = START_SPEED_Y;
	  break;
	case SDLK_DOWN: 
	  cursor.dir = DIR_DOWN; 
	  cursor.speed_y = START_SPEED_Y;
	  break;
	case SDLK_r: 
	  drawMap();
	  break;
	case SDLK_SPACE: 
	  startJump();
	  break;
	case SDLK_ESCAPE:
	  cursor.dir = DIR_QUIT;
	  break;
	}
	break;	
  case SDL_KEYUP: 
	switch(event->key.keysym.sym) {
	case SDLK_LEFT: 
	  if(cursor.dir == DIR_LEFT) cursor.dir = DIR_UPDATE;
	  break;
	case SDLK_RIGHT: 
	  if(cursor.dir == DIR_RIGHT) cursor.dir = DIR_UPDATE;
	  break;
	case SDLK_UP: 
	  if(cursor.dir == DIR_UP) cursor.dir = DIR_UPDATE;
	  break;
	case SDLK_DOWN: 
	  if(cursor.dir == DIR_DOWN) cursor.dir = DIR_UPDATE;
	  break;
	}
	break;
  }
}

GameCollisionCheck getGameCollisionCheck() {
  GameCollisionCheck check;
  check.start_x = cursor.pos_x - EXTRA_X;
  if(check.start_x < 0) check.start_x = 0;
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

int containsType(GameCollisionCheck *check, int type) {
  SDL_Rect rect;
  int x, y, n;
  for(y = check->start_y; y < check->end_y; y++) {
	for(x = check->start_x; x < check->end_x;) {
	  n = map.image_index[LEVEL_MAIN][x + (y * map.w)];
	  if(n > -1) {
		if(images[n]->type == type) {
		  rect.x = x;
		  rect.y = y;
		  rect.w = images[n]->image->w / TILE_W;
		  rect.h = images[n]->image->h / TILE_H;
		  if(intersects(&rect, &check->rect)) {
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

// return a 1 to proceed, 0 to stop
int detectCollision(int dir) {
  GameCollisionCheck check = getGameCollisionCheck();
  int ret = 1;
  // are we in a wall?
  ret = !containsType(&check, TYPE_WALL);
  return ret;
}

int detectLadder() {
  GameCollisionCheck check = getGameCollisionCheck();
  // are we smack on top of a ladder? (extend checking to 1 row below Tom)
  if(cursor.pixel_y == 0) {
	check.end_y++;
	if(check.end_y >= map.h) check.end_y = map.h;
	check.rect.h++;
  }
  return containsType(&check, TYPE_LADDER);
}

void runMap(char *name, int w, int h) {
  if(!initMap(name, w, h)) return;
  // try to load the map and quit if you can't find it.
  if(!loadMap(0)) {
	fprintf(stderr, "Can't find map file: %s\n", name);
	fflush(stderr);
	exit(0);
  }
  cursor.pos_x = 177;
  cursor.pos_y = 44;
  cursor.speed_x = 8;
  cursor.speed_y = 8;
  drawMap();
  // set our painting events
  map.beforeDrawToScreen = gameBeforeDrawToScreen;
  map.afterMainLevelDrawn = afterMainLevelDrawn;
  map.detectCollision = detectCollision;
  map.detectLadder = detectLadder;
  // add our event handling
  map.handleMapEvent = gameMainLoop;
  // activate gravity and accelerated movement
  map.accelerate = 1;
  map.gravity = 1;
  map.monsters = 1;

  // start the map main loop
  moveMap();
}

void initGame() {
  game.face = 0;
  game.dir = GAME_DIR_RIGHT;
}
