#include "Sound.h"

#define MUSIC_CHANNEL 0
#define DOOR_CHANNEL 1

int sound_enabled = 1, music_enabled = 1;
int sound_loaded = 0;

Mix_Chunk *sound[100];
Mix_Music *intro_music, *game_music;

void
playSound(int index)
{
  if(!sound_enabled)
    return;
  if(Mix_PlayChannel(DOOR_CHANNEL, sound[index], 0) == -1) {
    printf("Mix_PlayChannel: %s\n", Mix_GetError());
  }
}

void
playIntroMusic()
{
  if(!music_enabled)
    return;
  if(!intro_music)
    return;
  if(Mix_PlayMusic(intro_music, -1) == -1) {
    printf("Mix_PlayMusic: intro %s\n", Mix_GetError());
    // well, there's no music, but most games don't break without music...
  }
}

void
playGameMusic()
{
  if(!music_enabled)
    return;
  if(!game_music)
    return;
  if(Mix_PlayMusic(game_music, -1) == -1) {
    printf("Mix_PlayMusic: game %s\n", Mix_GetError());
    // well, there's no music, but most games don't break without music...
  }
}

void
loadSound(int index, char *name)
{
  char path[300];
  sprintf(path, "%s%s%s.wav", SOUND_DIR, PATH_SEP, name);
  fprintf(stderr, "Loading sound: path=%s\n", path);
  sound[index] = Mix_LoadWAV(path);
  if(!sound[index]) {
    printf("Mix_LoadWAV: name=%s path=%s error=%s\n", name, path,
           Mix_GetError());
    SDL_Quit();
  }
}

void
stopMusic()
{
  Mix_HaltMusic();
}

void
initAudio()
{
  char path[300];
  if(!sound_enabled)
    return;

  if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    printf("Mix_OpenAudio: %s\n", Mix_GetError());
    exit(2);
  }

  intro_music = game_music = NULL;
  sprintf(path, "%s%sintro.ogg", SOUND_DIR, PATH_SEP);
  intro_music = Mix_LoadMUS(path);
  if(!intro_music) {
    printf("Mix_LoadMUS(\"intro\"): %s\n", Mix_GetError());
  }
  showLoadingProgress();

  sprintf(path, "%s%sgame.ogg", SOUND_DIR, PATH_SEP);
  game_music = Mix_LoadMUS(path);
  if(!game_music) {
    printf("Mix_LoadMUS(\"game\"): %s\n", Mix_GetError());
  }
  showLoadingProgress();

  loadSound(DOOR_SOUND, "door");
  showLoadingProgress();
  loadSound(OBJECT_SOUND, "object");
  showLoadingProgress();
  loadSound(POP_SOUND, "pop");
  showLoadingProgress();
  loadSound(CLOSED_SOUND, "closed");
  showLoadingProgress();
  loadSound(COIL_SOUND, "spring");
  showLoadingProgress();
  loadSound(BUBBLE_SOUND, "bubble");
  showLoadingProgress();
  loadSound(MENU_SOUND, "menu");
  showLoadingProgress();
  loadSound(DEATH_SOUND, "death");
  showLoadingProgress();
  loadSound(PLATFORM_SOUND, "platform");
  showLoadingProgress();
  loadSound(JUMP_SOUND, "jump");
  showLoadingProgress();
  loadSound(GEM_SOUND, "gem");
  showLoadingProgress();

  Mix_AllocateChannels(16);
  sound_loaded = 1;

}
