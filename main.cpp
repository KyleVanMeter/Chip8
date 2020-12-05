#include "chip8.h"
#include <SDL2/SDL.h>

chip8 Chip;

int main(int argc, char **argv) {
  // SDL2 init stuff goes here
  // setupGraphics();
  // setupInput();

  // Clear chip8 memory, and copy program into memory
  Chip.init();
  Chip.load();

  for (;;) {
    Chip.emuCycle();

    // This flag is only set by two opcodes
    // 0x00E0 - Clear screen
    // 0xDXYN - Draw sprite
    if (Chip.drawFlag) {
      // drawGraphics();
    }

    // Store key states for each cycle
    Chip.setKeys();
  }

  return 0;
}
