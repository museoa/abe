#include "Game.h"

int getGameFace() {
  // change the face
  if(game.balloonTimer) {
	game.face = (game.dir == GAME_DIR_LEFT ? 6 : 7);
	game.balloonTimer--;
	if(game.balloonTimer <= 0) {
	  game.balloonTimer = 0;
	  map.gravity = 1;
	} else {
	  return game.face;
	}
  }
  if(cursor.dir == DIR_LEFT || cursor.dir == DIR_RIGHT) {
	game.face++;
	if(game.face >= FACE_COUNT * FACE_STEP) game.face = 0;
  }
  return (game.dir == GAME_DIR_LEFT ? 
		  (game.face / FACE_STEP) : 
		  (game.face / FACE_STEP) + FACE_COUNT);
}

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

  if(game.draw_player) {
	SDL_BlitSurface(tom[getGameFace()], 
					NULL, screen, &pos);
  }
}

void gameBeforeDrawToScreen() {
  char s[80];
  sprintf(s, "life %d score %d keys %d balloons %d", game.lives, game.score, game.keys, game.balloons);
  drawString(screen, 5, 5, s);
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
	case SDLK_RETURN:
	  if(!game.balloonTimer && game.balloons) {
		game.balloons--;
		game.balloonTimer = BALLOON_RIDE_INTERVAL;
		map.gravity = 0;
	  }
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

void handleDeath() {
  int i;

  fprintf(stderr, "Player death!\n");
  fflush(stderr);

  game.lives--;

  // Flash player. Don't move monsters during this.
  move_monsters = 0;
  for(i = 0; i < 5; i++) {
	game.draw_player = 0;
	drawMap();
	SDL_Delay(200);
	game.draw_player = 1;
	drawMap();
	SDL_Delay(200);
  }
  move_monsters = 1;

  if(game.lives <= 0) {
	cursor.dir = DIR_QUIT;
  } else {
	repositionCursor(game.player_start_x, game.player_start_y);
	cursor.speed_x = 0;
	cursor.speed_y = 0;
	drawMap();
  }
}

// return a 1 to proceed, 0 to stop
int detectCollision(int dir) {
  int n;
  Position pos, key;

  pos.pos_x = cursor.pos_x;
  pos.pos_y = cursor.pos_y;
  pos.pixel_x = cursor.pixel_x;
  pos.pixel_y = cursor.pixel_y;
  pos.w = tom[0]->w / TILE_W;
  pos.h = tom[0]->h / TILE_H;

  // did we hit a monster?
  if(!GOD_MODE && detectMonster(&pos)) {
	handleDeath();
	return 1;
  }

  // did we hit an object? 
  // FIXME: this fails if there many objects close together.
  // maybe it should return a NULL terminated array of positions.
  if(containsTypeWhere(&pos, &key, TYPE_OBJECT)) {
	// remove from map
	n = map.image_index[LEVEL_MAIN][key.pos_x + (key.pos_y * map.w)];
	if(n == img_key) {
	  game.keys++;
	} else if(n == img_balloon[0] || n == img_balloon[1] || n == img_balloon[2]) {
	  game.balloons++;
	}
	map.image_index[LEVEL_MAIN][key.pos_x + (key.pos_y * map.w)] = -1;
	map.redraw = 1;
  }

  // did we hit a door?
  if(containsTypeWhere(&pos, &key, TYPE_DOOR)) {
	if(game.keys > 0) {
	  // open door
	  map.image_index[LEVEL_MAIN][key.pos_x + (key.pos_y * map.w)] = -1;
	  map.image_index[LEVEL_FORE][key.pos_x + (key.pos_y * map.w)] = img_door2;
	  game.keys--;
	  map.redraw = 1;
	  return 1;
	} else {
	  return 0;
	}
  }
  
  // are we in a wall?
  return !containsType(&pos, TYPE_WALL);
}

int detectLadder() {
  Position pos;
  if(game.balloonTimer) return 1; // With a balloon you can move where you want

  pos.pos_x = cursor.pos_x;
  pos.pos_y = cursor.pos_y;
  pos.pixel_x = cursor.pixel_x;
  pos.pixel_y = cursor.pixel_y;
  pos.w = tom[0]->w / TILE_W;
  pos.h = tom[0]->h / TILE_H;
  // are we smack on top of a ladder? (extend checking to 1 row below Tom)
  if(pos.pixel_y == 0 && pos.pos_y + pos.h >= map.h) {
	pos.h++;
  }  
  return containsType(&pos, TYPE_LADDER);
}

void runMap(char *name, int w, int h) {
  resetMap();
  resetMonsters();

  // try to load the map and quit if you can't find it.
  if(!loadMap(0)) {
	fprintf(stderr, "Can't find map file: %s\n", name);
	fflush(stderr);
	return;
  }
  // start outside
  //  cursor.pos_x = 177;
  //  cursor.pos_y = 44;

  game.player_start_x = 20;
  game.player_start_y = 28;
  game.lives = 5;
  game.score = 0;
  game.draw_player = 1;
  game.keys = 0;
  game.balloons = 0;
  game.balloonTimer = 0;

  // start inside
  cursor.pos_x = game.player_start_x;
  cursor.pos_y = game.player_start_y;

  // monster testing
  //  cursor.pos_x = 35;
  //  cursor.pos_y = 44;

  cursor.speed_x = 8;
  cursor.speed_y = 8;

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
  drawMap();
  moveMap();
}

void initGame() {
  game.face = 0;
  game.dir = GAME_DIR_RIGHT;
  game.balloonTimer = 0;
}
