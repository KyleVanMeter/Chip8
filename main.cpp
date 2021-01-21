#include "chip8.h"
#include <SDL2/SDL.h>
#include <iostream>

#define HEIGHT 32 * 10
#define WIDTH 64 * 10

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


  // Clear chip8 memory, and copy program into memory
  Chip.init();
  Chip.load();

  for (;;) {
    Chip.emuCycle();

    // This flag is only set by two opcodes
    // 0x00E0 - Clear screen
    // 0xDXYN - Draw sprite
    if (Chip.drawFlag) {
      SDL_SetRenderDrawColor(render, 10, 10, 10, 255);
      SDL_RenderPresent(render);
      SDL_RenderClear(render);
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
