#include "Util.h"

int contains(SDL_Rect *a, int x, int y) {
  return (x >= a->x && x < a->x + a->w && 
		  y >= a->y && y < a->y + a->h ? 1 : 0);
}

int sectionIntersects(int a1, int a2, int b1, int b2) {
  return((a1 <= b1 && a2 > b1) || (a1 >= b1 && a1 < b2));
}

int intersects(SDL_Rect *a, SDL_Rect *b) {
  return(sectionIntersects(a->x, a->x + a->w, b->x, b->x + b->w) &&
		 sectionIntersects(a->y, a->y + a->h, b->y, b->y + b->h));
}
