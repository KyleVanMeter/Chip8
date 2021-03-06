#include <chrono>
#include <cstring>
#include <iostream>

#include "IReader.hpp"
#include "chip8.hpp"
#include "inputparser.hpp"
#include "spdlog/spdlog.h"
#include "view.hpp"

chip8 Chip;

#if defined(__WIN32__)
#define LOAD_LIB(libName) __pragma(comment(lib, libName))
//__pragma( message("linking " libName ".lib") )
LOAD_LIB("..\\SDL2\\lib\\x86\\sdl2.lib")
LOAD_LIB("..\\SDL2\\lib\\x86\\sdl2main.lib")
#endif

void printHelp() {
  std::cerr << "Usage: Chip8 -f [filename]\n             -l [0,1,2] Set log "
               "level (defaults to 1)\n";
}

int main(int argc, char **argv) {
  constexpr int HEIGHT = 32;
  constexpr int WIDTH = 64;
  constexpr int NUM_SQUARES = HEIGHT * WIDTH;

  InputParser input(argc, argv);

  if (input.CMDOptionExists("-f")) {
    if (input.CMDOptionExists("-l")) {
      int option = std::stoi(input.getCMDOption("-l"));

      if (option == 0) {
        spdlog::set_level(spdlog::level::off);
      }
      if (option == 1) {
        spdlog::set_level(spdlog::level::info);
      }
      if (option == 2) {
        spdlog::set_level(spdlog::level::trace);
      }
    } else {
      spdlog::set_level(spdlog::level::info);
    }

    std::string fileName = input.getCMDOption("-f");
    std::chrono::time_point<std::chrono::steady_clock> begin, end;

    // Clear chip8 memory, and copy program into memory
    Chip.init();
    FileReader reader(fileName);
    Chip.load(reader);

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
      spdlog::error("ERR: ", e.what());
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
