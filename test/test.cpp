#define CATCH_CONFIG_MAIN
#include "../IReader.hpp"
#include "../chip8.hpp"
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

  SECTION("opcode: 0x0010") { chip.emuCycle(); }
}

TEST_CASE("TestChip8 constructor.") {
  TestChip8 T = TestChip8();

  SECTION("Test initial values.") {
    REQUIRE(T.GetI() == 0);
    REQUIRE(T.GetSP() == 0);
    REQUIRE(T.GetPC() == 0x200);
  }
}

TEST_CASE("TestChip8 executes instructions.") {
  TestChip8 T = TestChip8();

  SECTION("OP_1NNN JP addr.") {
    std::vector<char> opcodes{(char)0x1F, (char)0xFF};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetPC() == 0xFFF);
  }

  SECTION("OP_2NNN CALL addr.") {
    std::vector<char> opcodes{(char)0x2F, (char)0xFF};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetSP() == 1);
    REQUIRE(T.GetPC() == 0xFFF);
    REQUIRE(T.GetStack(T.GetSP()) == 0x200);
  }

  SECTION("OP_3XKK SE Vx, byte.  Not Equal.") {
    std::vector<char> opcodes{(char)0x31, (char)0xFF};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetPC() == 0x202);
  }

  SECTION("OP_3XKK SE Vx, byte.  Equal.") {
    std::vector<char> opcodes{(char)0x31, (char)0x00};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetPC() == 0x204);
  }
}
