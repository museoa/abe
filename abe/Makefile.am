AUTOMAKE_OPTIONS = no-dependencies subdir-objects

noinst_PROGRAMS = \
	abe

abe_SOURCES = src/Main.c src/Image.c src/Trans.c src/Splash.c \
src/Editor.c src/Font.c src/Util.c src/Menu.c src/Map.c src/MapIO.c \
src/Sound.c src/Game.c src/Monster.c

abe_LDADD = @MATHLIB@ -lSDL_mixer

