/**
   This file handles scanning the images directory and loading images
   into memory. It also keeps track of named images' indexes.
 */
#include "Image.h"

/**
   Store the image in an array or a named img buffer.
 */
doLoadImage(char *filename, char *name) {
  fprintf(stderr, "\tLoading %s [%s]...\n", filename, name);
  fflush(stderr);

  SDL_Surface *image = SDL_LoadBMP(filename);
  if(image == NULL) {
	fprintf(stderr, "Couldn't load %s: %s\n", filename, SDL_GetError());
	fflush(stderr);
	return;
  }

  // set black as the transparent color key
  SDL_SetColorKey(image, SDL_SRCCOLORKEY, SDL_MapRGBA(image->format, 0x00, 0x00, 0x00, 0xff));

  // save the image
  if(!strcmp(name, "abe")) {
	title = image;
  } else if(!strcmp(name, "fonts")) {
	initFonts(image);
  } else if(!strcmp(name, "tom1")) {
	tom[0] = image;
  } else if(!strcmp(name, "tom2")) {
	tom[1] = image;
  } else if(!strcmp(name, "tom3")) {
	tom[2] = image;
  } else if(!strcmp(name, "tom4")) {
	tom[3] = image;
  } else if(!strcmp(name, "tom5")) {
	tom[4] = image;
  } else if(!strcmp(name, "tom6")) {
	tom[5] = image;
  } else {
	// a primitive hashtable
	int type = TYPE_WALL;
	if(!strcmp(name, "brick")) {
	  img_brick = image_count;
	} else if(!strcmp(name, "rock")) {
	  img_rock = image_count;
	} else if(!strcmp(name, "back")) {
	  img_back = image_count;
	} else if(!strcmp(name, "ladder")) {
	  type = TYPE_LADDER;
	}
	// store the image
	if(!(images[image_count] = malloc(sizeof(Image)))) {
	  fprintf(stderr, "Out of memory.");
	  fflush(stderr);
	  exit(0);
	}
	images[image_count]->image = image;
	images[image_count]->name = strdup(name);
	images[image_count]->type = type;
	image_count++;
  }
}

int selectDirEntry(const struct dirent *d) {
  return (strstr(d->d_name, ".bmp") || strstr(d->d_name, ".BMP") ? 1 : 0);
}

char *getImageName(char *s) {
  char *r = strdup(s);
  *(r + strlen(r) - 4) = 0;
  return r;
}

/**
   Load every bmp file from the ./images directory.
 */
void loadImages() {
  image_count = 0;

  fprintf(stderr, "Looking for images in %s \n", IMAGES_DIR);
  fflush(stderr);

  // it's important to always load the images in the same order.
  struct dirent **namelist;
  int n;
  n = scandir(IMAGES_DIR, &namelist, selectDirEntry, alphasort);
  if(n < 0) {
	fprintf(stderr, "Can't sort directory: %s\n", IMAGES_DIR);
	fflush(stderr);
	exit(0);
  } else {
	char *name;
	char path[300];
	while(n--) {
	  name = getImageName(namelist[n]->d_name);
	  sprintf(path, "%s/%s", IMAGES_DIR, namelist[n]->d_name);
	  doLoadImage(path, name);
	  free(name);
	  free(namelist[n]);
	}
	free(namelist);
  }
}
