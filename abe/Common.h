#ifndef COMMON_H 
#define COMMON_H

#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

typedef struct _position {
  int pos_x, pos_y; // in tiles
  int pixel_x, pixel_y; // in pixels
  int w, h; // in tiles
} Position;

#endif
