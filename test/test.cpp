#define CATCH_CONFIG_MAIN
#include "../IReader.hpp"
#include "../chip8.h"
#include <catch/catch.hpp>

TEST_CASE("Chip8 initializes.") {
  chip8 chip;
  chip.init();

  SECTION("Chip8 gfx is initialized to 0") {
    int acc = 0;
    for (int i = 0; i < 64 * 32; i++) {
      acc += chip.gfx[i];
    }

    REQUIRE(acc == 0);
  }

  SECTION("Chip8 drawFlag is initialized to false") { REQUIRE(!chip.drawFlag); }
}

TEST_CASE("TestReader loads.") {
  chip8 chip;
  chip.init();
  std::vector<char> opcodes{(char)0x00E0};

  TestReader reader(opcodes);

  SECTION("opcodes are copied.") {
    REQUIRE(reader.get().size() == opcodes.size());
  }

  SECTION("opcodes are the same.") {
    REQUIRE(reader.get().front() == opcodes.front());
  }
}

TEST_CASE("chip8 executes instructions.") {
  chip8 chip;
  chip.init();

  std::vector<char> opcodes{(char)0x61, (char)0xFF};
  TestReader reader(opcodes);

  chip.load(reader);

  SECTION("opcode: 0x0010") {
    chip.emuCycle();
  }
}
