#include "Menu.h"

/**
   event handling
 */
void menuMainLoop(SDL_Event *event) {
  // main loop events
  switch(event->type) {
  case SDL_KEYDOWN:
	if(event->key.keysym.sym == SDLK_RETURN) {
	  fprintf(stderr, "menu event!");
	  fflush(stderr);
	}
	break;
  }
}

void showMenu() {
  drawString(screen, 100, FONT_HEIGHT * 3, "start game");
  drawString(screen, 100, FONT_HEIGHT * 4, "map editor");
  drawString(screen, 100, FONT_HEIGHT * 5, "change settings");
  drawString(screen, 100, FONT_HEIGHT * 6, "about abe!!");
  SDL_Flip(screen);
}
