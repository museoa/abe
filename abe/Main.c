#include "Main.h"

#define RUNMODE_SPLASH 0
#define RUNMODE_EDITOR 1
#define RUNMODE_GAME 2

cleanUpAndExit() {
  if(state == STATE_SPLASH_SCREEN) {
	hideSplashScreen();
  }
}

void startEditor() {
  state = STATE_EDIT_LOOP;
  initEditor();
  editMap("default", 1000, 1000);
}

void startGame() {
  state = STATE_GAME_LOOP;
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

  while(1) {
	SDL_WaitEvent(&event);
	if(state == STATE_EDIT_LOOP) {
	  editorMainLoop(&event);
	} else if(state == STATE_GAME_LOOP) {
	  gameMainLoop(&event);
	} else if(state == STATE_MENU) {
	  menuMainLoop(&event);
	}
	// some global events
	switch(event.type) {
	case SDL_KEYDOWN:
	  if(event.key.keysym.sym == SDLK_ESCAPE) {
		if(state == STATE_SPLASH_SCREEN) hideSplashScreen();
		return;
	  } else if(event.key.keysym.sym == SDLK_SPACE && state == STATE_SPLASH_SCREEN) {
		hideSplashScreen();
		state = STATE_MAIN_LOOP;
		startGame();
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

void testWrite() {
  int map_size = LEVEL_COUNT * 1000 * 1000;
  int *buff[LEVEL_COUNT * 1000 * 1000];
  FILE *fp;
  int i;
  int w, h;

  // read map
  loadMap(0);

  // step1. map-compress it
  int new_size;
  printf("Compressing...\n");
  int *compressed_map = compressMap(&new_size);
  fprintf(stderr, "Compressed map. old_size=%ld new_size=%ld\n", map_size, new_size);
  fflush(stderr);

  // step2. further compress it into a file
  if(!(fp = fopen("maps/compressed.map", "wb"))) {
	printf("error writing.");
	exit(0);
  }
  fwrite(&map.w, sizeof(int), 1, fp);
  fwrite(&map.h, sizeof(int), 1, fp);
  compress(compressed_map, new_size, fp);
  fclose(fp);
  free(compressed_map);
}

void testRead() {
  int map_size = LEVEL_COUNT * 1000 * 1000;
  int read_buff[LEVEL_COUNT * 1000 * 1000];
  FILE *fp;
  int i;
  int w, h;

  // read step1. read and decompress map file
  if(!(fp = fopen("maps/compressed.map", "rb"))) {
	printf("error reading.");
	exit(0);
  }
  fread(&map.w, sizeof(int), 1, fp);
  fread(&map.h, sizeof(int), 1, fp);
  int count_read = decompress(read_buff, map_size, fp);
  fprintf(stderr, "read %d ints\n", count_read);
  fflush(stderr);
  fclose(fp);

  // read step2. mapDecompress it.
  decompressMap(read_buff);

  // write map read (for diff)
  if(!(fp = fopen("maps/map.dif", "wb"))) {
	printf("error writing.");
	exit(0);
  }
  fwrite(&map.w, sizeof(int), 1, fp);
  fwrite(&map.h, sizeof(int), 1, fp);
  for(i = 0; i < LEVEL_COUNT; i++) {
	fwrite(map.image_index[i], sizeof(int) * map.w * map.h, 1, fp);
  }
  fclose(fp);
}

void testCompression() {
  if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	exit(1);
  }

  screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  if(screen == NULL) {
	fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
	exit(1);
  }

  SDL_ShowCursor(0);

  loadImages();

  initMap("default", 1000, 1000);
  testWrite();
  testRead();
  exit(0);
}

main(int argc, char *argv[]) {
  testCompression();

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
