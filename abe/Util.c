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

/**
   Simple run-length compression.
   For n entries of the same value, if n > 3 then write (each entry an int):
   key n value
   (Key should not appear as a valid image_index, currently key is f0f0f0f0.)
   return 0 on error, number of entries written on success
*/
int compress(int *buff, size_t size, FILE *fp) {
  size_t size_written = 0;
#ifdef USE_COMPRESSION
  //  printf("Compressing...\n");
  int *block;
  size_t i, t = 0;
  unsigned int block_note = BLOCK_NOTE;
  if(!(block = (int*)malloc(sizeof(int) * size))) {
	fprintf(stderr, "Out of memory when writing compressed file");
	fflush(stderr);
	exit(-1);
  }
  for(i = 0; i < size; i++) {
	if(i > 0 && buff[i] != block[t - 1]) {
	  //	  printf("\tdiff. int found i=%ld t=%ld buff[i]=%ld block[t - 1]=%ld - ", i, t, buff[i], block[t - 1]);
	  if(t > 3) {
		//		printf("\tcompressed block\n");
		// compressed write
		fwrite(&block_note, sizeof(unsigned int), 1, fp);
		fwrite(&t, sizeof(size_t), 1, fp);
		fwrite(block, sizeof(int), 1, fp);
		size_written += 3;
	  } else {
		//		printf("\tnormal block\n");
		// normal write
		fwrite(block, sizeof(int), t, fp);
		size_written += t;
	  }
	  t = 0;
	}
	block[t++] = buff[i];
  }
  // write the last block
  //  printf("\tlast diff. int found i=%ld t=%ld buff[i]=%ld block[t - 1]=%ld - ", i, t, buff[i], block[t - 1]);
  if(t > 3) {
	//	printf("\tcompressed block\n");
	// compressed write
	fwrite(&block_note, sizeof(unsigned int), 1, fp);
	fwrite(&t, sizeof(size_t), 1, fp);
	fwrite(block, sizeof(int), 1, fp);
	size_written += 3;
  } else {
	//	printf("\tnormal block\n");
	// normal write
	fwrite(block, sizeof(int), t, fp);
	size_written += t;
  }
  free(block);
#else
  //  printf("Writing uncompressed...\n");
  fwrite(buff, size, 1, fp);
  size_written += size;
#endif
  return size_written;
}

/**
   Read above compression and decompress into buff.
   return number of items read into buff (should equal size on success)
*/
int decompress(int *buff, size_t size, FILE *fp) {
  int *block;
  int real_size;
  size_t i = 0, t = 0, r = 0, start = 0;
  size_t count;
  int value;
  unsigned int block_note = BLOCK_NOTE;

  // read the file
  if(!(block = (int*)malloc(sizeof(int) * size))) {
	fprintf(stderr, "Out of memory when writing compressed file");
	fflush(stderr);
	exit(-1);
  }
  real_size = fread(block, sizeof(int), size, fp);

#ifdef USE_COMPRESSION
  //  printf("Decompressing... real_size=%ld\n", real_size);
  for(t = 0; t < real_size;) {

	if((unsigned int)block[t] == BLOCK_NOTE) {
	  // write the previous block
	  if(start != t) {
		count = t - start;
		//		printf("\tuncompressed block: i=%ld start=%ld t=%d count=%ld\n", i, start, t, count);
		memcpy(buff + i, block + start, count * sizeof(int));
		i += count;
	  }

	  // write the compressed block
	  // (these are for readability.)
	  count = block[t + 1];
	  value = block[t + 2];
	  //	  printf("\tcompressed block start: i=%ld start=%ld t=%ld count=%ld\n", i, start, t, count);
	  for(r = 0; r < count; r++) {
		buff[i++] = value;
	  }
	  t += 3;
	  start = t;
	  //	  printf("\tcompressed block end: i=%ld start=%ld t=%ld count=%ld\n", i, start, t, count);
	} else {
	  t++;
	}
  }
  // write the last block
  if(start != t) {
	count = t - start;
	//	printf("\tuncompressed block: i=%ld start=%ld t=%d count=%ld\n", i, start, t, count);
	memcpy(buff + i, block + start, count * sizeof(int));
	i += count;
  }

  //  printf("\tdone: i=%ld start=%ld t=%d count=%ld\n", i, start, t, count);
  free(block);
  return i;
#else
  //  printf("Read uncompressed.");
  memcpy(buff, block, real_size * sizeof(int));
  free(block);
  return real_size * sizeof(int);
#endif
}
