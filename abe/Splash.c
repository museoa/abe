#include "Splash.h"
#include <math.h>

#define TITLE_X_STEP 5
#define RECT_WIDTH 40
#define RECT_HEIGHT 30
#define RAD_ANG (3.14159 / 180.0)
#define MUL 10

SDL_Surface *splash_back;
int title_x, title_y;

int moveTitle(int *title_x, int *title_y) {
  if(*title_x > 0) {
	*title_x -= 5;
	if(*title_x < 0) *title_x = 0;
	return 1;
  } else if(*title_y > 0) {
	*title_y -= 5;
	if(*title_y < 0) *title_y = 0;
	return 1;
  } else {
	return 0;
  }
}

void clearScreen(int title_x, int title_y) {
  SDL_Rect rect, title_pos;
  int x,y;

  title_pos.x = title_x;
  title_pos.y = title_y;
  title_pos.w = title->w;
  title_pos.h = title->h;
  
  for(y = 0; y < screen->h; y+=RECT_HEIGHT) {
	for(x = 0; x < screen->w; x+=RECT_WIDTH) {
	  rect.x = x;
	  rect.y = y;
	  rect.w = RECT_WIDTH;
	  rect.h = RECT_HEIGHT;
	  SDL_FillRect(splash_back, &rect,  SDL_MapRGBA(screen->format, 0x20, 0x90, 0x40, 0xff));
	  SDL_BlitSurface(splash_back, NULL, screen, NULL);

	  if(!(x % (RECT_WIDTH * 2))) {
		moveTitle(&title_x, &title_y);
		title_pos.x = title_x;
		title_pos.y = title_y;
	  }
	  SDL_BlitSurface(title, NULL, screen, &title_pos);
	}
	SDL_Flip(screen);
	SDL_Delay(5);
  }

  while(moveTitle(&title_x, &title_y)) {	  
	title_pos.x = title_x;
	title_pos.y = title_y;
	SDL_BlitSurface(title, NULL, screen, &title_pos);
	SDL_Flip(screen);
	SDL_Delay(10);
  }
}

/**
   Get the next point on a sine-wave.
   @x the x coordinate
   @y the y coordinate
   @param dir -1 is left, 1 is right
   @angle is in degrees
 */
void getNextPoint(int *x, int *y, int *dir, int *angle, int w) {
  *x += (TITLE_X_STEP * (*dir));
  *y = (int)(sin((double)(*angle) * RAD_ANG) * 200);
  *angle += 7;
  if(*angle > 360) *angle = 360 - *angle;
  if(*x <= 0) {
	*dir = 1;
	fflush(stderr);
  } else if(*x >= screen->w - w) {
	*dir = -1;
	fflush(stderr);
  }
}

/**
   Start the splash screen thread.
 */
void showSplashScreen() {
  SDL_Rect dest;
  int x, y, max_y;
  int angle = 45;
  int dir = -1;
  int tom_dir = 1;
  int tom_face = 0;
  int mode = 0;
  SDL_Rect tom_rect;
  SDL_Event event;

  // create the splash screen background
  if(!(splash_back = SDL_CreateRGBSurface(SDL_HWSURFACE, 
										  screen->w, screen->h, 
										  screen->format->BitsPerPixel, 
										  0, 0, 0, 0))) {
	fprintf(stderr, "Error creating surface: %s\n", SDL_GetError());
	fflush(stderr);
	return;
  }

  for(x = 0; x < screen->w; x+=images[img_back]->image->w) {
	for(y = 0; y < screen->h; y+=images[img_back]->image->h) {
	  dest.x = x;
	  dest.y = y;
	  dest.w = images[img_back]->image->w;
	  dest.h = images[img_back]->image->h;
	  SDL_BlitSurface(images[img_back]->image, NULL, splash_back, &dest);
	}
  }

  max_y = screen->h;
  for(x = 0; x < screen->w; x+=images[img_brick]->image->w) {
	if(max_y > 0) {
	  for(y = 0; y < max_y; y+=images[img_brick]->image->h) {
		dest.x = x;
		dest.y = y;
		dest.w = images[img_brick]->image->w;
		dest.h = images[img_brick]->image->h;
		SDL_BlitSurface(images[img_brick]->image, NULL, splash_back, &dest);
	  }
	}
	if(x < screen->w / 2) max_y-=images[img_brick]->image->h;
	else max_y+=images[img_brick]->image->h;
  }

  // start to bounce the title about the screen
  x = 300;
  y = 240;
  tom_rect.x = 0;
  tom_rect.y = screen->h - tom[0]->h;
  tom_rect.w = tom[0]->w;
  tom_rect.h = tom[0]->h;
  while(1) {

	// handle events
	while(SDL_PollEvent(&event)) {
	  if(event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
		case SDLK_RETURN: 
		  mode = 1;
		  break;
		case SDLK_SPACE:
		  mode = 2;
		  break;
		case SDLK_ESCAPE:
		  mode = 2;
		  break;
		}
	  }
	}
	if(mode) break;

	// cover the screen with bricks
	SDL_BlitSurface(splash_back, NULL, screen, NULL);

	// show the logo
	getNextPoint(&x, &y, &dir, &angle, title->w);
	dest.x = x;
	dest.y = 160 - y;
	dest.w = title->w;
	dest.h = title->h;
	SDL_BlitSurface(title, NULL, screen, &dest);
	title_x = x;
	title_y = 160 - y;

	// for every third step of title, move tom 1 step
	if(x % (TITLE_X_STEP * 3) == 0) {
	  tom_rect.x += 4 * tom_dir;
	  if(tom_rect.x + tom_rect.w >= screen->w) {
		tom_dir = -1;
		tom_rect.x += 4 * tom_dir;
		tom_face = 0;
	  }
	  if(tom_rect.x < 0) {
		tom_dir = 1;
		tom_rect.x += 4 * tom_dir;
		tom_face = 3;
	  }
	  tom_face++;
	  if(tom_dir == 1) {
		if(tom_face >= 6) tom_face = 3;
	  } else {
		if(tom_face >= 3) tom_face = 0;
	  }
	}
	// show tom
	SDL_BlitSurface(tom[tom_face], NULL, screen, &tom_rect);
	
	//SDL_UpdateRects(screen, 1, &screen->clip_rect);
	SDL_Flip(screen);
		
	//	SDL_Delay(y < 10 ? 10 : (int)((double)y * .7));
	SDL_Delay(25);
  }
  
  // start the game
  clearScreen(title_x, title_y);
  free(splash_back);

  //  switch(mode) {
  //  case 1: startEditor(); break;
  //  case 2: startGame(); break;
  //  default: return;
  //  }
  showMenu();
}
