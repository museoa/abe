#ifndef IMAGE_H 
#define IMAGE_H 

#include <sys/types.h>
#include <dirent.h>
#include "Main.h"

#define IMAGES_DIR "images"

SDL_Surface *title;
SDL_Surface *tom[6];

void loadImages();
void loadImagesFromTar();

#define TYPE_WALL 0
#define TYPE_LADDER 1

typedef struct image {
  char *name;
  SDL_Surface *image;
  int type;
} Image;

Image *images[256];
int image_count;

// known image indexes
int img_brick, img_rock, img_back;

#endif
