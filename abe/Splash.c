#include "Splash.h"

#define TITLE_X_STEP 5
#define RECT_WIDTH 40
#define RECT_HEIGHT 30
#define RAD_ANG (3.14159 / 180.0)
#define MUL 10

SDL_Surface *splash_back;
SDL_Thread *splash_thread;
int splash_running = 1;
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
   The splash screen thread main method.
 */
int splashThreadFunc(void *data) {
  int x = 300;
  int y = 240;
  int angle = 45;
  int dir = -1;
  SDL_Rect dest, tom_rect;
  tom_rect.x = 0;
  tom_rect.y = screen->h - tom[0]->h;
  int tom_dir = 1;
  tom_rect.w = tom[0]->w;
  tom_rect.h = tom[0]->h;
  int tom_face = 0;
  while(splash_running) {
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
  return 0;
}

/**
   Start the splash screen thread.
 */
void showSplashScreen() {
  // create the splash screen background
  if(!(splash_back = SDL_CreateRGBSurface(SDL_HWSURFACE, 
										  screen->w, screen->h, 
										  screen->format->BitsPerPixel, 
										  0, 0, 0, 0))) {
	fprintf(stderr, "Error creating surface: %s\n", SDL_GetError());
	fflush(stderr);
	return;
  }
  SDL_Rect dest;

  int x, y;
  for(x = 0; x < screen->w; x+=images[img_back]->image->w) {
	for(y = 0; y < screen->h; y+=images[img_back]->image->h) {
	  dest.x = x;
	  dest.y = y;
	  dest.w = images[img_back]->image->w;
	  dest.h = images[img_back]->image->h;
	  SDL_BlitSurface(images[img_back]->image, NULL, splash_back, &dest);
	}
  }

  int max_y = screen->h;
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
	if(x < screen->w / 2) max_y-=images[0]->image->h;
	else max_y+=images[img_brick]->image->h;
  }

  // start the thread to bounce the title about the screen
  splash_thread = SDL_CreateThread(splashThreadFunc, NULL);
  if(!splash_thread) {
	fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
	fflush(stderr);
	exit(0);
  }
}

/**
   Stop the splash screen and clear the screen.
 */
void hideSplashScreen() {
  // kill bounce thread
  if(splash_thread) {
	splash_running = 0;
	SDL_WaitThread(splash_thread, NULL);
  }

  clearScreen(title_x, title_y);

  free(splash_back);
  //free(splash_thread); <-- this hangs. Do I need to do this?
}
