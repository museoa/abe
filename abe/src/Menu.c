#include "Menu.h"

#define MAX_ENTRIES 50
#define MAX_CHOICES 10

#define SOUND_ENABLED 0
#define MUSIC_ENABLED 1
#define FULLSCREEN_ENABLED 2

typedef struct _settingEntry {
  char title[80];
  int choice_count;
  char choices[MAX_CHOICES][20];
  int selected;
} SettingEntry;
SettingEntry entries[] = {
  { "sound", 2, { "on", "off" }, 0 },
  { "music", 2, { "on", "off" }, 0 },
  { "full screen", 2, { "on", "off" }, 0 },
  { "", 0, { "" }, 0 }
};
int entry_count;
SDL_Surface *back;

// menu cursor
int m_menu_y;
#define BULLET_FACE_COUNT 4
int m_face = 0, m_face_mod = 4;

int getSelectionCharIndex(int n) {
  int t;
  int sum = 0;
  for(t = 0; t < entries[n].choice_count; t++) {
	if(t > 0) sum++;
	if(entries[n].selected == t) return sum;
	sum += strlen(entries[n].choices[t]);
  }
  return -1;
}

int getNumberOfCharWidthOfSelection(int n) {
  return strlen(entries[n].choices[entries[n].selected]);
}

void drawSettings() {
  int i, t, start, end;
  SDL_Rect pos;
  char s[200];
  for(i = 0; i < MAX_ENTRIES; i++) {
	if(!strlen(entries[i].title)) {
	  entry_count = i;
	  break;
	}
	drawString(screen, 35, (i + 2) * FONT_HEIGHT, entries[i].title);

	// concat choices
	strcpy(s, "");
	for(t = 0; t < entries[i].choice_count; t++) {
	  if(t > 0) strcat(s, " ");
	  strcat(s, entries[i].choices[t]);
	}

	start = getSelectionCharIndex(i);
	end = start + getNumberOfCharWidthOfSelection(i);

	// highlight the selected item
	pos.x = 250 + getFontPixelWidth(s, 0, start) - 2;
	pos.y = (i + 2) * FONT_HEIGHT;
	pos.w = getFontPixelWidth(s, start, end) + 4;
	pos.h = FONT_HEIGHT;
	SDL_FillRect(screen, &pos, SDL_MapRGBA(screen->format, 0xf0, 0xf0, 0x00, 0x00));

	// draw the choices
	drawString(screen, 250, (i + 2) * FONT_HEIGHT, s);
  }
}

int isEnabled(int n) {
  if(n == SOUND_ENABLED || n == MUSIC_ENABLED) 
	return sound_loaded;
}

void saveSettings() {
  int old_screen, old_music;

  old_screen = full_screen;
  old_music = music_enabled;

  sound_enabled = !(entries[SOUND_ENABLED].selected);
  music_enabled = !(entries[MUSIC_ENABLED].selected);
  if(old_music != music_enabled) {
	if(music_enabled) {
	  playIntroMusic();
	} else {
	  stopMusic();
	}
  }
  full_screen = !(entries[FULLSCREEN_ENABLED].selected);
  if(old_screen != full_screen) {
	SDL_WM_ToggleFullScreen(screen);
  }
}

void loadSettings() {
  entries[SOUND_ENABLED].selected = (sound_loaded && sound_enabled ? 0 : 1);
  entries[MUSIC_ENABLED].selected = (sound_loaded && music_enabled ? 0 : 1);
  entries[FULLSCREEN_ENABLED].selected = (full_screen ? 0 : 1);
}

void paintScreen() {
  SDL_Rect pos;

  saveSettings();
  SDL_BlitSurface(back, NULL, screen, NULL);
  drawString(screen, 0, 0, "abe!! settings");
  drawSettings();

  // draw cursor
  m_face++;
  if(m_face >= m_face_mod * BULLET_FACE_COUNT) m_face = 0;
  pos.x = 5;
  pos.y = (m_menu_y + 2) * FONT_HEIGHT;
  pos.w = images[img_bullet[0]]->image->w;
  pos.h = images[img_bullet[0]]->image->h;
  SDL_BlitSurface(images[img_bullet[m_face / m_face_mod]]->image, NULL, screen, &pos);

  SDL_Flip(screen);
}

void showSettings() {
  SDL_Event event;

  loadSettings();
  createBack(&back);  
  
  while(1) {
	paintScreen();

	while(SDL_PollEvent(&event)) {
	  switch(event.type) {
	  case SDL_KEYDOWN:
		switch(event.key.keysym.sym) {
		case SDLK_ESCAPE:
		  goto escape;
		case SDLK_DOWN:
		  m_menu_y++;
		  if(m_menu_y >= entry_count) m_menu_y = 0;
		  break;
		case SDLK_UP:
		  m_menu_y--;
		  if(m_menu_y < 0) m_menu_y = entry_count - 1;
		  break;
		case SDLK_SPACE:
		  if(isEnabled(m_menu_y)) {
			entries[m_menu_y].selected++;
			if(entries[m_menu_y].selected >= entries[m_menu_y].choice_count) 
			  entries[m_menu_y].selected = 0;
		  }
		  break;
		default:		  
		  break;
		}
		break;
	  default:
		break;
	  }
	}
	SDL_Delay(15);
  }
 escape:
  SDL_FreeSurface(back);
}
