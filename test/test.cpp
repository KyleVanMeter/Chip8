#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include "../chip8.h"

TEST_CASE( "Chip8 initializes." ) {
  chip8 chip;
  chip.init();

  SECTION( "Chip8 gfx is initialized to 0" ) {
    int acc = 0;
    for (int i = 0; i < 64*32; i++) {
      acc += chip.gfx[i];
    }

    REQUIRE(acc == 0);
  }

  SECTION( "Chip8 drawFlag is initialized to false" ) {
    REQUIRE(!chip.drawFlag);
  }
}
