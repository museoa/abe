#include <errno.h>
#include "MapIO.h"

void saveMap() {
  char path[300];
  FILE *fp;
  size_t new_size, written;
  Uint16 *compressed_map;
  char *err;

  sprintf(path, "%s%s%s.dat", MAPS_DIR, PATH_SEP, map.name);
  printf("Saving map %s\n", path);  
  fflush(stdout);

  if(!(fp = fopen(path, "wb"))) {
	err = strerror(errno);
	fprintf(stderr, "Can't open file for writing: %s\n", err);
	fflush(stderr);
	return;
  }
  // write the header
  fwrite(&(map.w), sizeof(map.w), 1, fp);
  fwrite(&(map.h), sizeof(map.h), 1, fp);

  // compression step 1
  printf("Compressing...\n");
  compressed_map = compressMap(&new_size);
  fprintf(stderr, "Compressed map. old_size=%ld new_size=%ld\n", (LEVEL_COUNT * map.w * map.h), new_size);
  fflush(stderr);
  // write out and further compress in step 2
  written = compress(compressed_map, new_size, fp);
  fprintf(stderr, "Compressed map step2. Written %ld ints. Compression ration: %f.2\%\n", written, 
		  (float)written / ((float)(LEVEL_COUNT * map.w * map.h) / 100.0));
  fflush(stderr);
  fclose(fp);
  free(compressed_map);
}

// call this after initMap()!
int loadMap(int draw_map) {
  char path[300];
  FILE *fp;
  size_t size;
  Uint16 *read_buff;
  int count_read;
  char *err;

  sprintf(path, "%s%s%s.dat", MAPS_DIR, PATH_SEP, map.name);
  printf("Loading map %s\n", path);  
  fflush(stdout);
  if(!(fp = fopen(path, "rb"))) {
	err = strerror(errno);
	fprintf(stderr, "Can't open file for reading: %s\n", err);
	fflush(stderr);
	return 0;
  }
  // read the header
  fread(&(map.w), sizeof(map.w), 1, fp);
  fread(&(map.h), sizeof(map.h), 1, fp);
  printf("map dimensions %dx%d\n", map.w, map.h);  
  fflush(stdout);

  // compression step 1: read compressed data from disk
  // FIXME: what would be nicer is to only allocate as much mem as used on disk.
  size = LEVEL_COUNT * map.w * map.h;
  printf("size %u\n", size);  
  fflush(stdout);
  if(!(read_buff = (Uint16*)malloc(sizeof(Uint16) * size))) {
	fprintf(stderr, "Out of memory on map read.");
	fflush(stderr);
	exit(0);
  }
  count_read = decompress(read_buff, size, fp);
  fprintf(stderr, "read %d ints\n", count_read);
  fflush(stderr);
  fclose(fp);
  
  // step 2: further uncompress
  decompressMap(read_buff);
  free(read_buff);

  resetCursor();
  if(draw_map) drawMap();
  return 1;
}

int convertMap(char *from, char *to) {
  FILE *fp;
  size_t size;
  int *buff;
  int count_read;
  char *err;
  int w, h, i;

  printf("Reading map to be converted %s\n", from);  
  fflush(stdout);
  if(!(fp = fopen(from, "rb"))) {
	err = strerror(errno);
	fprintf(stderr, "Can't open file for reading: %s\n", err);
	fflush(stderr);
	free(err);
	return 0;
  }
  // read the header
  fread(&(w), sizeof(w), 1, fp);
  fread(&(h), sizeof(h), 1, fp);

  if((buff = (int*)malloc(sizeof(int) * w * h)) == NULL) {
	fprintf(stderr, "Can't allocate memory!\n");
	fflush(stderr);
	return 0;
  }
  count_read = fread(buff, sizeof(int), w * h, fp);
  fprintf(stderr, "read %d ints\n", count_read);
  fflush(stderr);
  fclose(fp);

  // now throw away the second WORD and resave
  printf("Saving converting map %s\n", to);  
  fflush(stdout);
  if(!(fp = fopen(to, "wb"))) {
	err = strerror(errno);
	fprintf(stderr, "Can't open file for writing: %s\n", err);
	fflush(stderr);
	free(err);
	return 0;
  }
  // read the header
  fwrite(&w, sizeof(Uint16), 1, fp);
  fwrite(&h, sizeof(Uint16), 1, fp);
  for(i = 0; i < count_read; i++) {
	fwrite(buff + i, sizeof(Uint16), 1, fp);
  }
  fclose(fp); 
  free(buff);
  return 1;
}

/** Remove unnecesary EMPTY_MAPs. For example a 4 tile wide stone becomes a 1 int number.
	return new number of elements in new_size. (so num of bytes=new_size * sizeof(int)).
	caller must free returned pointer.
	call this method before calling Utils.compress(). This prepares the map
	for better compression by understanding the its structure. This doesn't 
	compress the map that much, but combined with Utils.compress() map files
	can go from 12M to 14K!
*/
Uint16 *compressMap(size_t *new_size) {
  Uint16 *q;
  Uint16 n;
  int level, i, x, y;
  size_t t = 0;

  if(!(q = (Uint16*)malloc(sizeof(Uint16) * map.w * map.h * LEVEL_COUNT))) {
	fprintf(stderr, "Out of memory in compressMap.");
	fflush(stderr);
	exit(-1);
  }
  for(level = 0; level < LEVEL_COUNT; level++) {
	for(y = 0; y < map.h; y++) {
	  for(x = 0; x < map.w;) {
		i = x + (y * map.w);
		n = map.image_index[level][i];
		*(q + t) = n;
		t++;
		if(n != EMPTY_MAP) {
		  // skip ahead
		  x += images[n]->image->w / TILE_W;
		} else {
		  x++;
		}
	  }
	}
  }
  *new_size = t;
  return q;
}

/**
   Decompress map by adding back missing -1-s. See compressMap() for
   details.
 */
void decompressMap(Uint16 *p) {
  int level, i, x, y, r;
  Uint16 n;
  size_t t = 0;

  for(level = 0; level < LEVEL_COUNT; level++) {
	for(y = 0; y < map.h; y++) {
	  for(x = 0; x < map.w;) {
		n = *(p + t);
		t++;
		i = x + (y * map.w);
		map.image_index[level][i] = n;
		x++;
		if(n != EMPTY_MAP) {
		  for(r = 1; r < images[n]->image->w / TILE_W && x < map.w; r++, x++) {
			map.image_index[level][i + r] = EMPTY_MAP;
		  }
		}
	  }
	}
  }
}
