#ifndef UTIL_H 
#define UTIL_H 

#include "Main.h"

// Rectangle math functions
int contains(SDL_Rect *a, int x, int y);
int intersects(SDL_Rect *a, SDL_Rect *b);

// Map compression functions (see fread/fwrite sign. assume size=sizeof(int))
int compress(int *buff, size_t size, FILE *fp);
int decompress(int *buff, size_t size, FILE *fp);

#define USE_COMPRESSION

// is this portable? assumes a 4 byte int.
// note: don't use ffffffff, it's -1
#define BLOCK_NOTE 0xf0f0f0f0

#endif
