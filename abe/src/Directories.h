#ifndef DIRECTORIES_H
#define DIRECTORIES_H

// The macros xstr(s) and str(s) were extracted from 
// http://gcc.gnu.org/onlinedocs/cpp/Stringification.html
// They are used to expand BASE_DIR .
#define xstr(s) str(s)
#define str(s) #s

#ifndef PATH_SIZE               // to allow modifying it by the compiler option -D
#define PATH_SIZE 1024
#endif

#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

// BASE_DIR has not " arround it.
#ifndef BASE_DIR
#define BASE_DIR .
#endif

#ifdef WIN32
#define SAVEGAME_DIR "savegame"
#else
#define SAVEGAME_DIR ".abe"
#endif

#define IMAGES_DIR "images"
#define MAPS_DIR "maps"
#define SOUND_DIR "sounds"

#endif
