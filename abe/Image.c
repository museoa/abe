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
	if(!strcmp(name, "brick")) {
	  img_brick = image_count;
	} else if(!strcmp(name, "rock")) {
	  img_rock = image_count;
	} else if(!strcmp(name, "back")) {
	  img_back = image_count;
	}
	// store the image
	if(!(images[image_count] = malloc(sizeof(Image)))) {
	  fprintf(stderr, "Out of memory.");
	  fflush(stderr);
	  exit(0);
	}
	images[image_count]->image = image;
	images[image_count]->name = strdup(name);
	images[image_count]->type = TYPE_WALL;
	image_count++;
  }
}

/**
   Load every bmp file from the ./images directory.
 */
void loadImages() {
  image_count = 0;

  fprintf(stderr, "Looking for images in %s \n", IMAGES_DIR);
  fflush(stderr);

  DIR *fp;
  if((fp = opendir(IMAGES_DIR)) == NULL) {
	fprintf(stderr, "Can't read directory: %s\n", IMAGES_DIR);
	fflush(stderr);
	exit(0);
  }

  char filename[300];
  struct dirent *d;

  while((d = readdir(fp)) != NULL) {
	if(strstr(d->d_name, ".bmp") || strstr(d->d_name, ".BMP")) {
	  sprintf(filename, "%s/%s", IMAGES_DIR, d->d_name);
	  *(d->d_name + strlen(d->d_name) - 4) = 0;
	  doLoadImage(filename, d->d_name);
	}
  }

  closedir(fp);
}
