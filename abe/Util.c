#include "Util.h"

int contains(SDL_Rect *a, int x, int y) {
  return (x >= a->x && x < a->x + a->w && 
		  y >= a->y && y < a->y + a->h ? 1 : 0);
}

int intersects(SDL_Rect *a, SDL_Rect *b) {
  return (contains(a, b->x, b->y) || contains(a, b->x + b->w - 1, b->y + b->h - 1) ||
		  contains(b, a->x, a->y) || contains(b, a->x + a->w - 1, a->y + a->h - 1));

}
