#include "Menu.h"

#define EVENT_NONE -1
#define EVENT_GAME 0
#define EVENT_EDITOR 1
#define EVENT_CHANGE 2
#define EVENT_ABOUT 3
#define EVENT_QUIT 4


#define POS_MAX 5

int pos = 0;

void drawScreen() {
  SDL_Rect p;

  SDL_FillRect(screen, NULL,  SDL_MapRGBA(screen->format, 0x20, 0x90, 0x40, 0xff));
  p.x = 50;
  p.y = FONT_HEIGHT * (pos + 5);
  p.w = screen->w - 100;
  p.h = FONT_HEIGHT;
  SDL_FillRect(screen, &p,  SDL_MapRGBA(screen->format, 0x00, 0x40, 0x00, 0xff));
  SDL_BlitSurface(title, NULL, screen, NULL);
  drawString(screen, 100, FONT_HEIGHT * 5, "start game");
  drawString(screen, 100, FONT_HEIGHT * 6, "map editor");
  drawString(screen, 100, FONT_HEIGHT * 7, "change settings");
  drawString(screen, 100, FONT_HEIGHT * 8, "about abe!!");
  drawString(screen, 100, FONT_HEIGHT * 9, "exit abe!!");
  SDL_Flip(screen);
}

void showMenu() {
  SDL_Event event;
  int e;

  // handle default runmode
  switch(runmode) {
  case RUNMODE_EDITOR:
	editMap();
	break;
  case RUNMODE_GAME:
	runMap();
	break;
  }
  
  // handle menu
  while(1) {
	drawScreen();

	e = EVENT_NONE;
	SDL_WaitEvent(&event);
	switch(event.type) {
	case SDL_KEYDOWN:
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE: e = EVENT_QUIT; break;
	  case SDLK_UP: pos--; break;
	  case SDLK_DOWN: pos++; break;
	  case SDLK_SPACE: case SDLK_RETURN: e = pos; break;
	  default:
		break;
	  }
	  break;
	}
	switch(e) {
	case EVENT_GAME:
	  runMap();
	  break;
	case EVENT_EDITOR:
	  editMap();
	  break;
	case EVENT_QUIT:
	  return;
	}
	if(pos < 0) pos = POS_MAX - 1;
	if(pos >= POS_MAX) pos = 0;
  }  
}
