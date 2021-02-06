#include <chrono>
#include <cstring>
#include <iostream>

#include "IChip.hpp"
#include "IReader.hpp"
#include "chip8.h"
#include "inputparser.h"
#include "view.h"

chip8 Chip;

void printHelp() { std::cerr << "Usage: Chip8 -f [filename]\n"; }

int main(int argc, char **argv) {
  constexpr int HEIGHT = 32;
  constexpr int WIDTH = 64;
  constexpr int NUM_SQUARES = HEIGHT * WIDTH;

  InputParser input(argc, argv);

  if (input.CMDOptionExists("-f")) {
    std::string fileName = input.getCMDOption("-f");
    std::chrono::time_point<std::chrono::steady_clock> begin, end;

    // Clear chip8 memory, and copy program into memory
    Chip.init();
    // Chip.load(fileName);
    FileReader reader(fileName);
    ChipParam param;
    Chip.load(reader, param);

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
        // to wait until at least 1/60th of a second has passed for each
        // emulation cycle for usability / accuracy.
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
      std::free(rgb);
      std::exit(-1);
    }

    std::free(rgb);
  } else if (input.CMDOptionExists("-h")) {
    printHelp();
  } else {
    printHelp();
  }

  return 0;
}
