#include "Sound.h"

#define MUSIC_CHANNEL 0
#define DOOR_CHANNEL 1

Mix_Chunk *sound[100];
Mix_Music *intro_music, *game_music;

void playSound(int index) {
  if(Mix_PlayChannel(DOOR_CHANNEL, sound[index], 0) == -1) {
    printf("Mix_PlayChannel: %s\n",Mix_GetError());    
  }
}

void playIntroMusic() {
  // play music forever
  // Mix_Music *music; // I assume this has been loaded already
  if(Mix_PlayMusic(intro_music, -1)==-1) {
    printf("Mix_PlayMusic: intro %s\n", Mix_GetError());
    // well, there's no music, but most games don't break without music...
  }
}

void playGameMusic() {
  // play music forever
  // Mix_Music *music; // I assume this has been loaded already
  if(Mix_PlayMusic(game_music, -1)==-1) {
    printf("Mix_PlayMusic: game %s\n", Mix_GetError());
    // well, there's no music, but most games don't break without music...
  }
}

void loadSound(int index, char *name) {
  char path[300];
  sprintf(path, "%s%s%s.wav", SOUND_DIR, PATH_SEP, name);
  fprintf(stderr, "Loading sound: path=%s\n", path);
  sound[index] = Mix_LoadWAV(path);
  if(!sound[index]) {
    printf("Mix_LoadWAV: name=%s path=%s error=%s\n", name, path, Mix_GetError());
	SDL_Quit();
  }
}

void initAudio() {
  char path[300];

  if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    printf("Mix_OpenAudio: %s\n", Mix_GetError());
    exit(2);
  }

  sprintf(path, "%s%sintro.xm", SOUND_DIR, PATH_SEP);
  intro_music = Mix_LoadMUS(path);
  if(!intro_music) {
    printf("Mix_LoadMUS(\"intro\"): %s\n", Mix_GetError());
	exit(2);
  }

  sprintf(path, "%s%sgame.xm", SOUND_DIR, PATH_SEP);
  game_music = Mix_LoadMUS(path);
  if(!game_music) {
    printf("Mix_LoadMUS(\"game\"): %s\n", Mix_GetError());
	exit(2);
  }  

  loadSound(DOOR_SOUND, "door");
  loadSound(OBJECT_SOUND, "object");
  loadSound(POP_SOUND, "pop");
  loadSound(CLOSED_SOUND, "closed");
  loadSound(COIL_SOUND, "spring");
  loadSound(BUBBLE_SOUND, "bubble");
  loadSound(MENU_SOUND, "menu");
  loadSound(DEATH_SOUND, "death");
  loadSound(PLATFORM_SOUND, "platform");
  loadSound(JUMP_SOUND, "jump");

  Mix_AllocateChannels(16);

}
