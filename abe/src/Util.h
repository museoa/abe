#ifndef UTIL_H 
#define UTIL_H 

#include "Main.h"

// Rectangle math functions
int contains(SDL_Rect *a, int x, int y);
int intersects(SDL_Rect *a, SDL_Rect *b);

#define USE_COMPRESSION

// is this portable? assumes a 4 byte int.
// note: don't use ffff, it's EMPTY_MAP
#define BLOCK_NOTE 0xf0f0

/**
   Simple run-length compression.
   For n entries of the same value, if n > 3 then write (each entry an int):
   key n value
   (Key should not appear as a valid image_index, currently key is f0f0f0f0.)

   Compression functions are like fread/fwrite and assume that size=sizeof(int)

   return 0 on error, number of ints written on success
*/
int compress(Uint16 *buff, size_t nmemb, FILE *fp);
int decompress(Uint16 *buff, size_t nmemb, FILE *fp);

#endif
