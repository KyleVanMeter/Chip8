#include "chip8.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

#define HEIGHT 32 * 10
#define WIDTH 64 * 10
#define NUM_SQUARES 2048

chip8 Chip;

void setupInput() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    std::exit(-1);
  }
}

int main(int argc, char **argv) {
  std::chrono::time_point<std::chrono::steady_clock> begin, end;
  setupInput();

  // SDL2 init stuff goes here
  SDL_Window *window =
      SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WIDTH, HEIGHT, 0);
  SDL_Renderer *render = SDL_CreateRenderer(window, -1, 0);

  // Setup each pixel as a rect so our display isn't horribly small.
  SDL_Rect rects[NUM_SQUARES];
  int iH = HEIGHT / 32;
  int iW = WIDTH / 32;
  int x = WIDTH / 2 - iW / 2;
  int y = HEIGHT / 2 - iH / 2;
  for (int i = 0; i < NUM_SQUARES; i++) {
    rects[i].x = i / 32 * iH;
    rects[i].y = i % 32 * iW / 2;
    rects[i].w = iW / 2;
    rects[i].h = iH;
  }

  // Clear chip8 memory, and copy program into memory
  Chip.init();
  Chip.load();

  for (;;) {
    begin = std::chrono::steady_clock::now();
    Chip.emuCycle();

    // This flag is only set by two opcodes
    // 0x00E0 - Clear screen
    // 0xDXYN - Draw sprite
    if (Chip.drawFlag) {
      for (int i = 0; i < NUM_SQUARES; i++) {
        if (Chip.gfx[i]) {
          SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
        } else {
          SDL_SetRenderDrawColor(render, 10, 10, 10, 255);
        }

        SDL_RenderFillRect(render, &rects[i]);
      }
      SDL_RenderPresent(render);
      SDL_RenderClear(render);
      Chip.drawFlag = false;
      // drawGraphics();
    }

    end = std::chrono::steady_clock::now();

    // TODO: Clean this up for obvious reasons!
    // 16 is approximately 1/60, or 60 Hz in ms.
    // this is a do/while loop because even if the condition isn't true we
    // should still execute the loop at least once.  Basically we are trying to
    // wait until at least 1/60th of a second has passed for each emulation
    // cycle for usability / accuracy.
    do {
      // Store key states for each cycle
      Chip.setKeys();
      end = std::chrono::steady_clock::now();
    } while (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
                 .count() <= 8);
  }

  SDL_DestroyRenderer(render);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
