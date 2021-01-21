#include "chip8.h"
#include <SDL2/SDL.h>
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

    // Store key states for each cycle
    Chip.setKeys();
  }

  SDL_DestroyRenderer(render);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
