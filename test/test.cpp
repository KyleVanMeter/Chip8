#define CATCH_CONFIG_RUNNER
#include "../IReader.hpp"
#include "../chip8.hpp"
#include "spdlog/spdlog.h"
#include <catch/catch.hpp>

int main(int argc, char **argv) {
  spdlog::set_level(spdlog::level::err);

  int result = Catch::Session().run(argc, argv);

  return result;
}

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

    REQUIRE(T.GetOpcode() == (Opcodes::Chip8::OP_1NNN | 0x0FFF));
    REQUIRE(T.GetPC() == 0xFFF);
  }

  SECTION("OP_2NNN CALL addr.") {
    std::vector<char> opcodes{(char)0x2F, (char)0xFF};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::Chip8::OP_2NNN | 0x0FFF));
    REQUIRE(T.GetSP() == 1);
    REQUIRE(T.GetPC() == 0xFFF);
    REQUIRE(T.GetStack(T.GetSP()) == 0x200);
  }

  SECTION("OP_3XKK SE Vx, byte.  Not Equal.") {
    std::vector<char> opcodes{(char)0x31, (char)0xFF};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::Chip8::OP_3XNN | 0x01FF));
    REQUIRE(T.GetPC() == 0x202);
  }

  SECTION("OP_3XKK SE Vx, byte.  Equal.") {
    std::vector<char> opcodes{(char)0x31, (char)0x00};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetPC() == 0x204);
  }

  SECTION("OP_4XKK SNE Vx, byte.  Not Equal.") {
    std::vector<char> opcodes{(char)0x41, (char)0xFF};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::Chip8::OP_4XNN | 0x01FF));
    REQUIRE(T.GetPC() == 0x204);
  }

  SECTION("OP_4XKK SNE Vx, byte.  Equal.") {
    std::vector<char> opcodes{(char)0x41, (char)0x00};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::Chip8::OP_4XNN | 0x0100));
    REQUIRE(T.GetPC() == 0x202);
  }

  /*
  SECTION("OP_5XY0 SE Vx, Vy.  Equal.") {
    std::vector<char> opcodes{(char)0x50, (char)0x00};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::Chip8::OP_5XY0));
    REQUIRE(T.GetV(1) == T.GetV(1));
    REQUIRE(T.GetPC() == 0x202);
  }
  */

  // SECTION("OP_5XY0 SE Vx, Vy.  Not Equal.") {}

  SECTION("OP_6XNN LD Vx, byte.") {
    std::vector<char> opcodes{(char)0x61, (char)0xEE};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::OP_6XNN | 0x01EE));
    REQUIRE(T.GetV(1) == 0x00EE);
    REQUIRE(T.GetPC() == 0x202);

    opcodes = {(char)0x31, (char)0xEE};
    TestReader reader2(opcodes);
    T.load(reader2);

    T.emuCycle();

    REQUIRE(T.GetPC() == 0x204);
  }

  SECTION("OP_7XNN ADD Vx, byte.") {
    std::vector<char> opcodes{(char)0x72, (char)0xEE};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetPC() == 0x202);
    REQUIRE(T.GetV(2) == 0xEE);
  }

  SECTION("OP_7XNN ADD Vx, byte.  Non-zero register value.") {
    std::vector<char> opcodes{(char)0x62, (char)0x11, (char)0x72, (char)0x22};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();
    REQUIRE(T.GetOpcode() == (Opcodes::OP_6XNN | 0x0211));
    REQUIRE(T.GetV(2) == 0x0011);
    REQUIRE(T.GetPC() == 0x202);
    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::OP_7XNN | 0x0222));
    REQUIRE(T.GetPC() == 0x204);
    REQUIRE(T.GetV(2) == (0x0011 + 0x0022));
  }

  SECTION("OP_8XY0 LD Vx, Vy.") {
    std::vector<char> opcodes{(char)0x80, (char)0x10};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();

    REQUIRE(T.GetOpcode() == (Opcodes::OP_8XY0 | 0x0010));
    REQUIRE(T.GetPC() == 0x202);
    REQUIRE(T.GetV(1) == T.GetV(0));
  }

  SECTION("OP_8XY0 LD Vx, Vy.  Non-zero register copy.") {
    std::vector<char> opcodes{(char)0x61, (char)0xEE, (char)0x80, (char)0x10};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();
    REQUIRE(T.GetOpcode()  == (Opcodes::OP_6XNN | 0x01EE));
    REQUIRE(T.GetPC() == 0x202);
    REQUIRE(T.GetV(1) == 0xEE);

    T.emuCycle();
    REQUIRE(T.GetOpcode() == (Opcodes::OP_8XY0 | 0x0010));
    REQUIRE(T.GetPC() == 0x204);
    REQUIRE(T.GetV(0) == T.GetV(1));
  }

  SECTION("OP_8XY1 Or Vx, Vy.") {
    std::vector<char> opcodes{(char)0x80, (char)0x11};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();
    REQUIRE(T.GetOpcode() == (Opcodes::OP_8XY1 | 0x0011));
    REQUIRE(T.GetPC() == 0x202);
    REQUIRE(T.GetV(0) == (0x00 | 0x01));
  }

  SECTION("OP_8XY1 Or Vx, Vy.  Non-zero register bitwise OR") {
    std::vector<char> opcodes{(char)0x60, (char)0x02, (char)0x61,
                              (char)0x19, (char)0x80, (char)0x11};
    TestReader reader(opcodes);
    T.load(reader);

    T.emuCycle();
    REQUIRE(T.GetOpcode() == (Opcodes::OP_6XNN | 0x0002));
    REQUIRE(T.GetPC() == 0x202);
    REQUIRE(T.GetV(0) == 0x02);

    T.emuCycle();
    REQUIRE(T.GetOpcode() == (Opcodes::OP_6XNN | 0x0119));
    REQUIRE(T.GetPC() == 0x204);
    REQUIRE(T.GetV(1) == 0x19);

    T.emuCycle();
    REQUIRE(T.GetOpcode() == (Opcodes::OP_8XY1 | 0x0011));
    REQUIRE(T.GetPC() == 0x206);
    REQUIRE(T.GetV(0) == (0x02 | 0x19));
  }
}
