#include "Main.h"

cleanUpAndExit() {
  if(state == STATE_SPLASH_SCREEN) {
	hideSplashScreen();
  }
}

void startEditor() {
  state = STATE_EDIT_LOOP;
  initEditor();
  editMap("default.map", 1000, 1000);
}

/**
   Main event handling.
*/
mainLoop(int showsplash) {
  SDL_Event event;

  if(showsplash) {
	showSplashScreen();
  } else {
	//		showMenu();
	//		state = STATE_MENU;
	state = STATE_MAIN_LOOP;
	startEditor();
  }

  while(1) {
	SDL_WaitEvent(&event);
	if(state == STATE_EDIT_LOOP) {
	  editorMainLoop(&event);
	} else if(state == STATE_MENU) {
	  menuMainLoop(&event);
	}
	// some global events
	switch(event.type) {
	case SDL_KEYDOWN:
	  if(event.key.keysym.sym == SDLK_ESCAPE) {
		if(state == STATE_SPLASH_SCREEN) hideSplashScreen();
		return;
	  } else if(event.key.keysym.sym == SDLK_RETURN && state == STATE_SPLASH_SCREEN) {
		hideSplashScreen();
		//		showMenu();
		//		state = STATE_MENU;
		state = STATE_MAIN_LOOP;
		startEditor();
	  }
	  break;
	case SDL_QUIT:
	  cleanUpAndExit();
	  return;
	}
  }
}

main(int argc, char *argv[]) {
  int showsplash = 1;

  if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	exit(1);
  }

  Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  int i;
  for(i = 0; i < argc; i++) {
	if(!strcmp(argv[i], "--fullscreen") || !strcmp(argv[i], "-f")) {
	  flags |= SDL_FULLSCREEN;
	} else if(!strcmp(argv[i], "--nosplash") || !strcmp(argv[i], "-n")) {
	  showsplash = 0;
	} else if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-?")) {
	  printf("Abe!! Happy Birthday, 2002\n\n");
	  printf("-f --fullscreen   Run in fullscreen mode.\n");
	  printf("-n --nosplash     Skip the splash screen.\n"); 
	  printf("-? --help         Show this help message.\n");
	  exit(0);
	}
  }

  screen = SDL_SetVideoMode(640, 480, 16, flags);
  if(screen == NULL) {
	fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
	exit(1);
  }

  SDL_ShowCursor(0);

  loadImages();

  mainLoop(showsplash);
  
  atexit(SDL_Quit);
}
