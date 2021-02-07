#ifndef __CHIP8_H__
#define __CHIP8_H__

#include "IReader.hpp"
#include <array>
#include <map>

static const unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

namespace Opcodes {
enum Chip8 {
  OP_001N = 0x0010,
  // OP_00BN = 0x00B0,
  // OP_00CN = 0x00C0,
  OP_00E0 = 0x00E0,
  OP_00EE = 0x00EE,
  OP_00FA = 0x000A,
  // OP_00FB = 0x0000,
  // OP_00FC = 0x0000,
  // OP_00FD = 0x0000,
  // OP_00FE = 0x0000,
  // OP_00FF = 0x0000,
  OP_0NNN = 0x0000,
  // OP_02A0 = 0x0000,
  OP_1NNN = 0x1000,
  OP_2NNN = 0x2000,
  OP_3XNN = 0x3000,
  OP_4XNN = 0x4000,
  OP_5XY0 = 0x5000,
  // OP_5XY1 = 0x5001,
  // OP_5XY2 = 0x5002,
  // OP_5XY3 = 0x5003,
  OP_6XNN = 0x6000,
  OP_7XNN = 0x7000,
  OP_8XY0 = 0x8000,
  OP_8XY1 = 0x8001,
  OP_8XY2 = 0x8002,
  OP_8XY3 = 0x8003,
  OP_8XY4 = 0x8004,
  OP_8XY5 = 0x8005,
  OP_8XY6 = 0x8006,
  OP_8XY7 = 0x8007,
  OP_8XYE = 0x800E,
  OP_9XY0 = 0x9000,
  // OP_9XY1 = 0x9001,
  // OP_9XY2 = 0x9002,
  // OP_9XY3 = 0x9003,
  OP_ANNN = 0xA000,
  OP_BNNN = 0xB000,
  // OP_B0NN = 0xB000, Ambiguous???
  // OP_B1X0 = 0xB100,
  // OP_B1X1 = 0xB101,
  // OP_BXY0 = 0xB000,
  // OP_BXYN = 0xB000,
  OP_CXNN = 0xC000,
  // OP_DXY0 = 0xD000,
  OP_DXYN = 0xD000,
  OP_EX9E = 0xE00E,
  OP_EXA1 = 0xE001,
  // OP_EXF2 = 0xE0F2,
  // OP_EXF5 = 0xE0F5,
  OP_FX07 = 0xF007,
  OP_FX0A = 0xF00A,
  OP_FX15 = 0xF015,
  OP_FX18 = 0xF008,
  OP_FX1E = 0xF00E,
  OP_FX29 = 0xF009,
  // OP_FX30 = 0xF030,
  OP_FX33 = 0xF003,
  OP_FX55 = 0xF055,
  OP_FX65 = 0xF065,
  // OP_FX75 = 0xF075,
  // OP_FX85 = 0xF085,
  // OP_FX94 = 0xF094,
  // OP_FXFB = 0xF0FB,
  // OP_FXF8 = 0xF0F8
};
static const Chip8 All[] = {
    OP_001N, OP_00E0, OP_00EE, OP_00FA, OP_0NNN, OP_1NNN, OP_2NNN, OP_3XNN,
    OP_4XNN, OP_5XY0, OP_6XNN, OP_7XNN, OP_8XY0, OP_8XY1, OP_8XY2, OP_8XY3,
    OP_8XY4, OP_8XY5, OP_8XY6, OP_8XY7, OP_8XYE, OP_9XY0, OP_ANNN, OP_BNNN,
    OP_CXNN, OP_DXYN, OP_EX9E, OP_EXA1, OP_FX07, OP_FX0A, OP_FX15, OP_FX18,
    OP_FX1E, OP_FX29, OP_FX33, OP_FX55, OP_FX65};

char EndianSwap(Chip8 in);
} // namespace Opcodes

class TestChip8;

class chip8 {
public:
  void init();
  // void load(std::string fileName);
  void load(IReader &reader);
  void emuCycle();
  void setKeys();

  bool drawFlag;
  unsigned char gfx[64 * 32];

  static std::map<unsigned char, unsigned char> HexToFontCharLoc;

  friend class TestChip8;

  void OP_001N();
  void OP_00E0();
  void OP_00EE();
  void OP_00FA();
  void OP_0NNN();
  void OP_1NNN();
  void OP_2NNN();
  void OP_3XNN();
  void OP_4XNN();
  void OP_5XY0();
  void OP_6XNN();
  void OP_7XNN();
  void OP_8XY0();
  void OP_8XY1();
  void OP_8XY2();
  void OP_8XY3();
  void OP_8XY4();
  void OP_8XY5();
  void OP_8XY6();
  void OP_8XY7();
  void OP_8XYE();
  void OP_9XY0();
  void OP_ANNN();
  void OP_BNNN();
  void OP_CXNN();
  void OP_DXYN();
  void OP_EX9E();
  void OP_EXA1();
  void OP_FX07();
  void OP_FX0A();
  void OP_FX15();
  void OP_FX18();
  void OP_FX1E();
  void OP_FX29();
  void OP_FX33();
  void OP_FX55();
  void OP_FX65();
  void OP_UNHANDLED();

private:
  unsigned int opcode;
  unsigned char memory[4096];
  unsigned char V[16];

  unsigned short I;
  unsigned short PC;
  unsigned short SP;
  unsigned short stack[16];

  std::array<unsigned char, 16> key;

  unsigned char delay_timer;
  unsigned char sound_timer;
};

class TestChip8 {
public:
  TestChip8();
  void load(IReader &reader);
  void emuCycle();

  unsigned int GetOpcode() const;

  unsigned char GetMemory(int index) const;
  unsigned char GetV(int index) const;

  unsigned short GetI() const;
  unsigned short GetPC() const;
  unsigned short GetSP() const;
  unsigned short GetStack(int index) const;

private:
  chip8 chip;
};

#endif
