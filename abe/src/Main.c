#include "Main.h"

int runmode;

SDL_Surface *screen;
int state;

void testModesInFormat(SDL_PixelFormat *format) {
  SDL_Rect **modes;
  int i;

  printf("Available hardware accelerated, fullscreen modes in %d bpp:\n", format->BitsPerPixel);

  /* Get available fullscreen/hardware modes */
  modes=SDL_ListModes(format, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_DOUBLEBUF);
  
  /* Check is there are any modes available */
  if(modes == (SDL_Rect **)0){
	printf("\tNo modes available!\n");
	return;
  }
  
  /* Check if our resolution is restricted */
  if(modes == (SDL_Rect **)-1){
	printf("\tAll resolutions available.\n");
  }
  else{
	/* Print valid modes */
	for(i=0;modes[i];++i)
	  printf("\t%d x %d\n", modes[i]->w, modes[i]->h);
  }

  free(modes);
}

void testModes() {
  SDL_PixelFormat format;

  format.BitsPerPixel=16;
  testModesInFormat(&format);
  format.BitsPerPixel=24;
  testModesInFormat(&format);
  format.BitsPerPixel=32;
  testModesInFormat(&format);
}

int main(int argc, char *argv[]) {
  Uint32 flags = SDL_DOUBLEBUF;
  int i;
  int width, height, bpp, n;
  int hw_mem = 1;
  int intro = 0;
  char *mapname;
  int mapwidth, mapheight;
  int window = 0;

  runmode = RUNMODE_SPLASH;  

  width = 640;
  height = 480;
  bpp = 16;

  if(SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	exit(1);
  }

  for(i = 0; i < argc; i++) {
	if(!strcmp(argv[i], "--window")) {
	  window = 1;
	} else if(!strcmp(argv[i], "--system") || !strcmp(argv[i], "-s")) {
	  hw_mem = 0;
	} else if(!strcmp(argv[i], "--editor") || !strcmp(argv[i], "-e")) {
	  runmode = RUNMODE_EDITOR;
	} else if(!strcmp(argv[i], "--nosound")) {
	  sound_enabled = 0;
	} else if(!strcmp(argv[i], "--intro") || !strcmp(argv[i], "-i")) {
	  runmode = RUNMODE_EDITOR;
	  intro = 1;
	} else if(!strcmp(argv[i], "--game") || !strcmp(argv[i], "-g")) {
	  runmode = RUNMODE_GAME;
	} else if((!strcmp(argv[i], "--width") || !strcmp(argv[i], "-w")) && i < argc - 1) {
	  n = atoi(argv[i + 1]);
	  if(n >= 320 && n <= 800) width=n;
	} else if((!strcmp(argv[i], "--height") || !strcmp(argv[i], "-h")) && i < argc - 1) {
	  n = atoi(argv[i + 1]);
	  if(n >= 200 && n <= 600) height=n;
	} else if((!strcmp(argv[i], "--bpp") || !strcmp(argv[i], "-b")) && i < argc - 1) {
	  n = atoi(argv[i + 1]);
	  if(n == 15 || n == 16 || n == 24 || n == 32) bpp = n;
	} else if(!strcmp(argv[i], "--test") || !strcmp(argv[i], "-t")) {
	  testModes();
	  exit(0);
	} else if(!strcmp(argv[i], "--convert")) {
	  convertMap(argv[i + 1], argv[i + 2]);
	  exit(0);
	} else if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-?") || !strcmp(argv[i], "-h")) {
	  printf("Abe!! Happy Birthday, 2002\n\n");
	  printf("--window           Run in windowed mode.\n");
	  printf("-e --editor        Skip the splash screen and run the editor.\n"); 
	  printf("-i --intro         Edit intro map.\n");
	  printf("-g --game          Skip the splash screen and run the game.\n"); 
	  printf("-t --test          Test video modes only.\n"); 
	  printf("-s --system        Use system memory instead of video(default) memory.\n");
	  printf("-w --width #       Use this width for the video mode.\n"); 
	  printf("-h --height #      Use this height for the video mode.\n"); 
	  printf("-b --bpp #         Use this bpp for the video mode.\n"); 
	  printf("--nosound          Don't use sound.\n"); 
	  printf("-? -h --help       Show this help message.\n");
	  exit(0);
	}
  }

  // the default map
  if(runmode != RUNMODE_SPLASH && !intro) {
	mapname = strdup("default");
	mapwidth = 1000;
	mapheight = 1000;
  } else {
	mapname = strdup("intro");
	mapwidth = 640 / TILE_W;
	mapheight = 480 / TILE_H;
  }

  if(hw_mem) {
	flags |= SDL_HWSURFACE;
  } else {
	flags |= SDL_SWSURFACE;
  }
  if(!window) flags |= SDL_FULLSCREEN;

  fprintf(stderr, "Attempting to set %dx%dx%d video mode.\n", width, height, bpp);
  fflush(stderr);
  screen = SDL_SetVideoMode(width, height, bpp, flags);
  if(screen == NULL) {
	fprintf(stderr, "Unable to set %dx%dx%d video: %s\n", width, height, bpp, SDL_GetError());
	exit(1);
  }
  fprintf(stderr, "Success:\n");
  fprintf(stderr, "\tSDL_HWSURFACE =%s\n", (screen->flags & SDL_HWSURFACE ? "true" : "false"));
  fprintf(stderr, "\tSDL_FULLSCREEN=%s\n", (screen->flags & SDL_FULLSCREEN ? "true" : "false"));
  fprintf(stderr, "\tSDL_DOUBLEBUF =%s\n", (screen->flags & SDL_DOUBLEBUF ? "true" : "false"));
  fprintf(stderr, "\tw=%d h=%d bpp=%d pitch=%d\n", screen->w, screen->h, screen->format->BitsPerPixel, screen->pitch);
  fflush(stderr);

  SDL_ShowCursor(0);

  initAudio();

  initMonsters();

  loadImages();

  initEditor();

  initGame();
  
  if(intro) {
	initMap("intro", 640 / TILE_W, 480 / TILE_H);
	editMap();
  } else {
	showIntro();
  }

  atexit(SDL_Quit);
  
  return 0;
}
