#ifndef EDITOR_H 
#define EDITOR_H 

#include <sys/types.h>
#include <dirent.h>
#include "Main.h"

#define EDIT_PANEL_HEIGHT 120

typedef struct _editpanel {
  SDL_Surface *image;
  int level;
  int image_index;
} EditPanel;

EditPanel edit_panel;

void initEditor();
void editMap(char *name, int w, int h);
void editorMainLoop(SDL_Event *event);
void drawEditPanel();

#endif
