CXX=g++
SDL_LIB=-L/usr/lib -lSDL2 -Wl,-rpath=/usr/lib
SDL_INCLUDE=-I/usr/include
HEADER=-I.
CXXFLAGS=-Wall -std=c++11 $(SDL_INCLUDE)
DEPS=chip8.h view.h IReader.hpp
OBJ=chip8.o main.o view.o IReader.o
RM=rm -f

LDFLAGS = $(SDL_LIB)

%.o: %.c $(DEPS)
	$(CXX) $(HEADER) $(CXXFLAGS) -c -o $@ $<

Chip8: $(OBJ)
	$(CXX) $(HEADER) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)

.PHONY: Chip8 tests

tests: $(OBJ)
	$(CXX) $(CXXFLAGS) chip8.o IReader.o $(LDFLAGS) -o ctest test/test.cpp

clean:
	$(RM) $(OBJ)

distclean:
	$(RM) Chip8
