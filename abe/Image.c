/**
   This file handles scanning the images directory and loading images
   into memory. It also keeps track of named images' indexes.
 */
#include "Image.h"

#define TAR_BLOCK_SIZE 512
#define TAR_NAME_SIZE 100
#define TAR_SIZE_SIZE 12
#define TAR_SIZE_OFFSET 124

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
	} else if(!strcmp(name, "crab1")) { // add monster images
	  addMonsterImage(MONSTER_CRAB, image_count);
	} else if(!strcmp(name, "crab2")) {
	  addMonsterImage(MONSTER_CRAB, image_count);
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
  loadImagesFromTar();

  /*
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
  */
}

/**
   Loading from the tar has several benefits:
   -only 1 file to deal with
   -keeps the order of images constant
   -allows me to append new images to the tar while the 
    order of the existing ones doesn't change.
 */
void loadImagesFromTar() {
  image_count = 0;

  char tmp_path[300];
  sprintf(tmp_path, "%s/%s", IMAGES_DIR, "tmp.bmp");
  FILE *tmp;

  char path[300];
  sprintf(path, "%s/%s", IMAGES_DIR, "images.tar");
  fprintf(stderr, "Opening %s for reading.\n", path);
  fflush(stderr);
  FILE *fp = fopen(path, "rb");
  if(!fp) {
	fprintf(stderr, "Can't open tar file.\n");
	fflush(stderr);
	exit(-1);
  }
  char buff[TAR_BLOCK_SIZE]; // a tar block
  int end = 0;
  int i;
  int mode = 0; // 0-header, 1-file
  char name[TAR_NAME_SIZE + 1], size[TAR_SIZE_SIZE + 1];
  long filesize;
  int found;
  int blocks_read;
  int block = 0;
  while(1) {
	if(fread(buff, 1, TAR_BLOCK_SIZE, fp) < TAR_BLOCK_SIZE) break; // EOF or error
	if(!mode) {
	  // are we at the end?
	  found = 0;
	  for(i = 0; i < TAR_BLOCK_SIZE; i++) {
		if(buff[i]) {
		  found = 1;
		  break;
		}
	  }	
	  if(!found) {
		end++;
		// a tar file ends with 2 NULL blocks
		if(end >= 2) break;
	  } else {
		// Get the name
		memcpy(name, buff, TAR_NAME_SIZE);
		// add a NUL if needed
		found = 0;
		for(i = 0; i < TAR_NAME_SIZE; i++) {
		  if(!name[i]) {
			found = 1;
			break;
		  }
		}
		if(!found) name[TAR_NAME_SIZE] = '\0';
		// Remove the .tmp
		*(name + strlen(name) - 4) = 0;
		// Get the size
		memcpy(size, buff + TAR_SIZE_OFFSET, TAR_SIZE_SIZE);
		size[TAR_SIZE_SIZE] = '\0';
		filesize = strtol(size, NULL, 8);
		blocks_read = filesize / TAR_BLOCK_SIZE + (filesize % TAR_BLOCK_SIZE ? 1 : 0);
		fprintf(stderr, "Found: >%s< size=>%ld< blocks=>%d<\n", name, filesize, blocks_read);
		fflush(stderr);
		mode = 1;

		// open a temp file to extract this image to
		if(!(tmp = fopen(tmp_path, "wb"))) {
		  fprintf(stderr, "Cannot open temp file for writing: %s\n", tmp_path);
		  fflush(stderr);
		  exit(0);
		}
	  }
	} else {
	  blocks_read--;

	  // write to temp file
	  fwrite(buff, (!blocks_read ? filesize % TAR_BLOCK_SIZE : TAR_BLOCK_SIZE), 1, tmp);

	  if(!blocks_read) {
		mode = 0;
		fclose(tmp);
		
		// load the image
		doLoadImage(tmp_path, name);
	  }
	}
	block++;
  }
  fclose(fp);

  // remove the tmp file
  remove(tmp_path);
}
