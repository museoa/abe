SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS=$(SDL_CFLAGS)
LDFLAGS=$(SDL_LDFLAGS)

SRC=Main.c Image.c Trans.c Splash.c Editor.c Font.c Util.c Menu.c Map.c Game.c Monster.c
HDR=Main.h Image.h Trans.h Splash.h Editor.h Font.h Util.h Menu.h Map.h Game.h Monster.h Common.h

CXX=gcc

all: abe

abe: $(SRC)
	$(CXX) -o abe $(CFLAGS) $(LDFLAGS) $(SRC)

clean:
	rm abe
