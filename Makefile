CXX=g++
SDL_LIB=-L/usr/lib -lSDL2 -Wl,-rpath=/usr/lib
SDL_INCLUDE=-I/usr/include
HEADER=-I.
CXXFLAGS=-Wall -std=c++11 $(SDL_INCLUDE)
DEPS=chip8.h
OBJ=chip8.o main.o
RM=rm -f

LDFLAGS = $(SDL_LIB)

%.o: %.c $(DEPS)
	$(CXX) $(HEADER) $(CXXFLAGS) -c -o $@ $<

Chip8: $(OBJ)
	$(CXX) $(HEADER) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)

.PHONY: Chip8

clean:
	$(RM) $(OBJ)

distclean:
	$(RM) Chip8
