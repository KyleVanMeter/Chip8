#include <chrono>
#include <cstring>
#include <iostream>

#include "chip8.h"
#include "view.h"

#define HEIGHT 32
#define WIDTH 64
#define NUM_SQUARES 2048

chip8 Chip;

int main(int argc, char **argv) {
  std::chrono::time_point<std::chrono::steady_clock> begin, end;

  // Clear chip8 memory, and copy program into memory
  Chip.init();
  Chip.load("PONG");

  View viewer("Chip8", WIDTH, HEIGHT, 10);
  unsigned char *rgb = static_cast<unsigned char *>(
      std::calloc(NUM_SQUARES * 3, sizeof(unsigned char)));

  viewer.SetFrame(rgb, HEIGHT);

  try {
    for (;;) {
      begin = std::chrono::steady_clock::now();
      Chip.emuCycle();

      if (Chip.drawFlag) {
        viewer.CopyToRGB24(Chip.gfx, rgb);
        viewer.SetFrame(rgb, HEIGHT);
        viewer.Update();

        Chip.drawFlag = false;
      }

      // TODO: Clean this up for obvious reasons!
      // 16 is approximately 1/60, or 60 Hz in ms.
      // this is a do/while loop because even if the condition isn't true we
      // should still execute the loop at least once.  Basically we are trying
      // to wait until at least 1/60th of a second has passed for each emulation
      // cycle for usability / accuracy.
      end = std::chrono::steady_clock::now();
      do {
        Chip.setKeys();
        end = std::chrono::steady_clock::now();
      } while (
          std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
              .count() <= 2);
    }
  } catch (std::exception &e) {
    std::cerr << "ERR: " << e.what();
    std::exit(-1);
  }

  return 0;
}
