#include "Sound.h"

#define MUSIC_CHANNEL 0
#define DOOR_CHANNEL 1

Mix_Chunk *door;

void playDoor() {
  if(Mix_PlayChannel(DOOR_CHANNEL, door, 0) == -1) {
    printf("Mix_PlayChannel: %s\n",Mix_GetError());    
  }
}

void initAudio() {
  char path[300];

  if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
    printf("Mix_OpenAudio: %s\n", Mix_GetError());
    exit(2);
  }

  sprintf(path, "%s%sdoor.wav", SOUND_DIR, PATH_SEP);
  door = Mix_LoadWAV(path);
  if(!door) {
    printf("Mix_LoadWAV: %s\n", Mix_GetError());
	exit(2);
  }

  Mix_AllocateChannels(16);
}
