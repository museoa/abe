#include "Main.h"

#define RUNMODE_SPLASH 0
#define RUNMODE_EDITOR 1
#define RUNMODE_GAME 2

void startEditor() {
  initEditor();
  editMap("default", 1000, 1000);
}

void startGame() {
  initGame();
  runMap("default", 1000, 1000);
}

/**
   Main event handling.
*/
mainLoop(int runmode) {
  SDL_Event event;

  switch(runmode) {
  case RUNMODE_EDITOR:
	startEditor();
	break;
  case RUNMODE_GAME:
	startGame();
	break;
  default:
	showSplashScreen();
  }
}

main(int argc, char *argv[]) {
  int runmode = RUNMODE_SPLASH;

  if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	exit(1);
  }

  Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  int i;
  for(i = 0; i < argc; i++) {
	if(!strcmp(argv[i], "--fullscreen") || !strcmp(argv[i], "-f")) {
	  flags |= SDL_FULLSCREEN;
	} else if(!strcmp(argv[i], "--editor") || !strcmp(argv[i], "-e")) {
	  runmode = RUNMODE_EDITOR;
	} else if(!strcmp(argv[i], "--game") || !strcmp(argv[i], "-g")) {
	  runmode = RUNMODE_GAME;
	} else if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-?")) {
	  printf("Abe!! Happy Birthday, 2002\n\n");
	  printf("-f --fullscreen   Run in fullscreen mode.\n");
	  printf("-e --editor       Skip the splash screen and run the editor.\n"); 
	  printf("-g --game         Skip the splash screen and run the game.\n"); 
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

  mainLoop(runmode);
  
  atexit(SDL_Quit);
}
