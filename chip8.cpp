#include "chip8.h"
#include "IReader.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string.h>
#include <vector>

#include <SDL2/SDL.h>

std::string intToString(int in) {
  std::stringstream ss;
  ss << std::hex << in;
  return ss.str();
}

char Opcodes::EndianSwap(Chip8 in) {
  return ((in & 0xFF00) >> 8) | ((in & 0x00FF) << 8);
  // memory[PC] << 8 | memory[PC + 1];
}

static std::map<unsigned char, unsigned char> KeySymToIndex{
    {49, 0},   {50, 1},   {51, 2},  {52, 3},  {113, 4},  {119, 5},
    {101, 6},  {114, 7},  {97, 8},  {115, 9}, {100, 10}, {102, 11},
    {122, 12}, {120, 13}, {99, 14}, {118, 15}};

std::map<unsigned char, unsigned char> chip8::HexToFontCharLoc{
    {0x1, 0},  {0x2, 5},  {0x3, 10}, {0x4, 15}, {0x5, 20},
    {0x6, 25}, {0x7, 30}, {0x8, 35}, {0x9, 40}, {0xA, 45},
    {0xB, 50}, {0xC, 55}, {0xD, 60}, {0xE, 65}, {0xF, 70}};

/*
 * Initialize registers, and memory once zeroed
 */
void chip8::init() {
  /*
  this->key.fill(0);

  // Program starts at 0x200, reset opcode, index, and stack pointer to 0
  PC = 0x200;
  opcode = 0;
  I = 0;
  SP = 0;
  */

  // load fontset
  std::memcpy(params.memory, fontset, sizeof fontset);
}

/*
void chip8::load(std::string fileName) {
  std::ifstream file(fileName, std::ios::binary | std::ios::in);
  if (file.good()) {
    std::vector<char> bytes((std::istreambuf_iterator<char>(file)),
                            (std::istreambuf_iterator<char>()));
    std::memcpy(memory + 0x200, bytes.data(), bytes.size());
  } else {
    throw std::runtime_error("File '" + fileName + "' not found.");
  }
}
*/
void chip8::load(IReader &reader, IChipParam &param) {
  params = param;
  std::vector<char> data = reader.get();
  std::memcpy(params.memory + 0x200, data.data(), data.size());
}

/*
 * Fetch, decode, execute opcodes, and update timers
 */
void chip8::emuCycle() {
  std::default_random_engine gen(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> distribution(0, 255);

  params.opcode = params.memory[params.PC] << 8 | params.memory[params.PC + 1];

  switch (params.opcode & 0xF000) {
  case 0x0000:
    switch (params.opcode & 0x00FF) {
    case Opcodes::Chip8::OP_0NNN:
      OP_0NNN();
      break;
    case Opcodes::Chip8::OP_001N:
      OP_001N();
      break;
    case Opcodes::Chip8::OP_00E0:
      OP_00E0();
      break;
    case Opcodes::Chip8::OP_00EE:
      OP_00EE();
      break;
    case Opcodes::Chip8::OP_00FA:
      OP_00FA();
      break;
    default:
      throw std::runtime_error(
          "Unknown opcode in 0: " + intToString(params.opcode) + "\n");
    }
    break;
  case Opcodes::Chip8::OP_1NNN:
    OP_1NNN();
    break;
  case Opcodes::Chip8::OP_2NNN:
    OP_2NNN();
    break;
  case Opcodes::Chip8::OP_3XNN:
    OP_3XNN();
    break;
  case Opcodes::Chip8::OP_4XNN:
    OP_4XNN();
    break;
  case Opcodes::Chip8::OP_5XY0:
    OP_5XY0();
    break;
  case Opcodes::Chip8::OP_6XNN:
    OP_6XNN();
    break;
  case Opcodes::Chip8::OP_7XNN:
    OP_7XNN();
    break;
  case 0x8000:
    switch (params.opcode & 0xF00F) {
    case Opcodes::Chip8::OP_8XY0:
      OP_8XY0();
      break;
    case Opcodes::Chip8::OP_8XY1:
      OP_8XY1();
      break;
    case Opcodes::Chip8::OP_8XY2:
      OP_8XY2();
      break;
    case Opcodes::Chip8::OP_8XY3:
      OP_8XY3();
      break;
    case Opcodes::Chip8::OP_8XY4: {
      OP_8XY4();
      break;
    }
    case Opcodes::Chip8::OP_8XY5:
      OP_8XY5();
      break;
    case Opcodes::Chip8::OP_8XY6:
      OP_8XY6();
      break;
    case Opcodes::Chip8::OP_8XY7:
      OP_8XY7();
      break;
    case Opcodes::Chip8::OP_8XYE:
      OP_8XYE();
      break;
    default:
      throw std::runtime_error(
          "Unknown opcode in 8: " + intToString(params.opcode) + "\n");
    }
    break;
  case Opcodes::Chip8::OP_9XY0:
    OP_9XY0();
    break;
  case Opcodes::Chip8::OP_ANNN:
    OP_ANNN();
    break;
  case Opcodes::Chip8::OP_BNNN:
    OP_BNNN();
    break;
  case Opcodes::Chip8::OP_CXNN: {
    OP_CXNN();
    break;
  }
  case Opcodes::Chip8::OP_DXYN: {
    OP_DXYN();
    break;
  }
  case 0xE000:
    switch (params.opcode & 0xF00F) {
    case Opcodes::Chip8::OP_EX9E:
      OP_EX9E();
      break;
    case Opcodes::Chip8::OP_EXA1:
      OP_EXA1();
      break;
    default:
      throw std::runtime_error(
          "Unknown opcode in E: " + intToString(params.opcode) + "\n");
    }
    break;
  case 0xF000:
    switch (opcode & 0xF00F) {
    case Opcodes::Chip8::OP_FX07:
      OP_FX07();
      break;
    case Opcodes::Chip8::OP_FX0A:
      OP_FX0A();
      break;
    case Opcodes::Chip8::OP_FX18:
      OP_FX18();
      break;
    case Opcodes::Chip8::OP_FX1E:
      OP_FX1E();
      break;
    case Opcodes::Chip8::OP_FX29:
      OP_FX29();
      break;
    case Opcodes::Chip8::OP_FX33:
      OP_FX33();
      break;
    case 0xF005:
      switch (params.opcode & 0xF0FF) {
      case Opcodes::Chip8::OP_FX15:
        OP_FX15();
        break;
      case Opcodes::Chip8::OP_FX55:
        OP_FX55();
        break;
      case Opcodes::Chip8::OP_FX65:
        OP_FX65();
        break;
      default:
        throw std::runtime_error(
            "Unknown opcode in F005: " + intToString(params.opcode) + "\n");
      }
      break;
    default:
      throw std::runtime_error(
          "Unknown opcode in F: " + intToString(params.opcode) + "\n");
    }
    break;
  default:
    throw std::runtime_error("Unknown opcode: " + intToString(params.opcode) +
                             "\n");
  }

  if (params.delay_timer > 0) {
    --params.delay_timer;
  }

  if (params.sound_timer > 0) {
    if (params.sound_timer == 1) {
      std::cout << "BEEP!\n";
    }

    --params.sound_timer;
  }
}

/*
 * Modifies the keys[] array to update keyboard input
 */
void chip8::setKeys() {
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      std::exit(0);
      break;
    case SDL_KEYDOWN:
      if (KeySymToIndex.find(e.key.keysym.sym) != KeySymToIndex.end()) {
        params.key[KeySymToIndex[e.key.keysym.sym]] = 1;
      }
      break;
    case SDL_KEYUP:
      if (KeySymToIndex.find(e.key.keysym.sym) != KeySymToIndex.end()) {
        params.key[KeySymToIndex[e.key.keysym.sym]] = 0;
      }
    default:
      std::cout << "Unhandled event.\n";
    }
  }
}

void chip8::OP_001N() { OP_UNHANDLED(); }

void chip8::OP_00E0() {
  std::cout << PC << " Clear display.\n";

  std::memset(gfx, 0, sizeof(gfx));
  PC += 2;
}

void chip8::OP_00EE() {
  std::cout << PC << " Return from subroutine at " << stack[SP] << "\n";

  PC = stack[SP];
  --SP;
  PC += 2;
}

void chip8::OP_00FA() { OP_UNHANDLED(); }

void chip8::OP_0NNN() {
  // Ignored
  std::cout << PC << " " << std::hex << opcode << " Ignored OP_0NNN\n";
  PC += 2;
}

void chip8::OP_1NNN() {
  std::cout << PC << " Jump to " << ((opcode & 0x0FFF)) << "\n";

  PC = opcode & 0x0FFF;
}

void chip8::OP_2NNN() {
  std::cout << PC << " Call subroutine at " << (opcode & 0x0FFF) << "\n";

  ++SP;
  stack[SP] = PC;
  PC = (opcode & 0x0FFF);
}

void chip8::OP_3XNN() {
  std::cout << PC << " Skip if V[" << ((opcode & 0x0F00) >> 8)
            << "] == " << (opcode & 0x00FF) << "\n";

  if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
    PC += 4;
  } else {
    PC += 2;
  }
}

void chip8::OP_4XNN() {
  std::cout << PC << " Skip if V[" << ((opcode & 0x0F00) >> 8)
            << "] != " << (opcode & 0x00FF) << "\n";

  if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
    PC += 4;
  } else {
    PC += 2;
  }
}

void chip8::OP_5XY0() { OP_UNHANDLED(); }

void chip8::OP_6XNN() {
  // Load immediate value NN into VX
  std::cout << PC << " Load immediate value " << (opcode & 0x00FF) << " into V["
            << ((opcode & 0x0F00) >> 8) << "]\n";
  V[((opcode & 0x0F00) >> 8)] = (opcode & 0x00FF);
  PC += 2;
}

void chip8::OP_7XNN() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to "
            << ((opcode & 0x00FF)) << " + V[" << ((opcode & 0x0F00) >> 8)
            << "]\n";

  V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
  PC += 2;
}

void chip8::OP_8XY0() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] = V["
            << ((opcode & 0x00F0) >> 4) << "].";

  V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
  PC += 2;
}

void chip8::OP_8XY1() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to V["
            << ((opcode & 0x0F00) >> 8) << "] | V[" << ((opcode & 0x00F0) >> 4)
            << "]\n";

  V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
  PC += 2;
}

void chip8::OP_8XY2() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to "
            << " V[" << ((opcode & 0x0F00) >> 8) << "] & V["
            << ((opcode & 0x00F0) >> 4) << "]\n";

  V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
  PC += 2;
}

void chip8::OP_8XY3() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to "
            << " V[" << ((opcode & 0x0F00) >> 8) << "] ^ V["
            << ((opcode & 0x00F0) >> 4) << "]\n";

  V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
  PC += 2;
}

void chip8::OP_8XY4() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to V["
            << ((opcode & 0x0F00) >> 8) << "] + V[" << ((opcode & 0x00F0) >> 4)
            << "].  ";

  if (V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4] > 255) {
    std::cout << "V[f] = 1\n";
    V[0xF] = 1;
  } else {
    std::cout << "V[f] = 0\n";
    V[0xF] = 0;
  }
  V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
  PC += 2;
}

void chip8::OP_8XY5() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to V["
            << ((opcode & 0x0F00) >> 8) << "] - V[" << ((opcode & 0x00F0) >> 4)
            << "].  ";

  if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
    std::cout << "V[f] = 1\n";
    V[0xF] = 1;
  } else {
    std::cout << "V[f] = 0\n";
    V[0xF] = 0;
  }

  V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
  PC += 2;
}

void chip8::OP_8XY6() { OP_UNHANDLED(); }

void chip8::OP_8XY7() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to V["
            << ((opcode & 0x00F0) >> 4) << "] - V[" << ((opcode & 0x0F00) >> 8)
            << "].  ";

  if (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]) {
    std::cout << "V[f] = 1\n";
    V[0xF] = 1;
  } else {
    std::cout << "V[f] = 0\n";
    V[0xF] = 0;
  }

  V[(opcode & 0x0F00) >> 8] =
      V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
  PC += 2;
}

void chip8::OP_8XYE() {
  std::cout << PC << " Set V[0xF] to MSB of V[" << ((opcode & 0x0F00) >> 8)
            << ", then multiply it by 2.\n";

  V[0xF] = V[((opcode & 0x0F00) >> 8)] > 0x80;
  V[(opcode & 0x0F00) >> 8] <<= 1;
  PC += 2;
}

void chip8::OP_9XY0() {
  std::cout << PC << " Skip next instruction if V[" << ((opcode & 0x0F00) >> 8)
            << "] != V[" << ((opcode & 0x00F0) >> 4) << "].  ";

  if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
    std::cout << "Skipped!\n";
    PC += 4;
  } else {
    std::cout << "Not skipped!\n";
    PC += 2;
  }
}

void chip8::OP_ANNN() {
  // Set I to NNN
  std::cout << PC << " Set I to " << (opcode & 0x0FFF) << "\n";

  I = opcode & 0x0FFF;
  PC += 2;
}

void chip8::OP_BNNN() { OP_UNHANDLED(); }

void chip8::OP_CXNN() {
  std::default_random_engine gen(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> distribution(0, 255);

  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8)
            << "] = Random byte & " << ((opcode & 0x00FF) >> 4) << "\n";

  V[(opcode & 0x0F00) >> 8] = distribution(gen) & ((opcode & 0x00FF) >> 4);
  PC += 2;
}

void chip8::OP_DXYN() {
  // Display n-byte sprite starting at I @ (V[X], V[Y]), & V[F] = 1
  unsigned short x = V[(opcode & 0x0F00) >> 8];
  unsigned short y = V[(opcode & 0x00F0) >> 4];
  unsigned short n = opcode & 0x000F;
  unsigned short pixel;

  std::cout << PC << " Display " << n << "-byte sprite starting at " << I
            << " @ (V[" << ((opcode & 0x0F00) >> 8) << "], V["
            << ((opcode & 0x00F0) >> 4) << "])\n";

  // Collision by default is 0
  V[0xF] = 0;

  for (int yline = 0; yline < n; yline++) {
    pixel = memory[I + yline];
    for (int xline = 0; xline < 8; xline++) {
      // if current evaluated pixel is set to 1
      if ((pixel & (0x80 >> xline)) != 0) {
        // if current displayed pixel is set to 1 set V[0xF] = 1
        if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
          V[0xF] = 1;
        }

        // XOR eval pixel with display pixel
        gfx[(x + xline + ((y + yline) * 64)) % (64 * 32)] ^= 1;
      }
    }
  }

  drawFlag = true;
  PC += 2;
}

void chip8::OP_EX9E() {
  // Skip next instruction if V[X] is pressed
  std::cout << PC << " Skip next instruction if V[" << ((opcode & 0x0F00) >> 8)
            << "] is pressed.";

  if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
    std::cout << "  Skipped!";
    PC += 4;
  } else {
    std::cout << "  Not skipped!";
    PC += 2;
  }
  std::cout << std::endl;
}

void chip8::OP_EXA1() {
  // Skip next instruction if V[X] is pressed
  std::cout << PC << " Skip next instruction if V[" << ((opcode & 0x0F00) >> 8)
            << "] is not pressed.";

  if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
    std::cout << "  Not skipped!";
    PC += 2;
  } else {
    std::cout << "  Skipped!";
    PC += 4;
  }
  std::cout << std::endl;
}

void chip8::OP_FX07() {
  std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8)
            << "] to delay_timer (" << delay_timer << ")\n";

  V[(opcode & 0x0F00) >> 8] = delay_timer;
  PC += 2;
}

void chip8::OP_FX0A() {
  std::cout << PC << " Wait for keypress...\n";

  // Potential problem here: As we have implemented so far several keys can
  // be pressed at once.  I am not sure if that is typical for chip8
  // emulators, or is not expected to be possible by some ROMs.  Either way
  // this presents a problem as we can only store one key value in V[x], and
  // have no way of judging which key that should be in the case of multiple
  // values.  Therefore we pick the first result.
  auto result = std::find(key.begin(), key.end(), 1);
  if (result != std::end(key)) {
    std::cout << "    Key pressed.  Setting V[" << ((opcode & 0x0F00) >> 8)
              << "] to 1.\n";
    V[(opcode & 0x0F00) >> 8] = std::distance(key.begin(), result);
    PC += 2;
  }
}

void chip8::OP_FX15() {
  std::cout << PC << " Set delay timer to V[" << ((opcode & 0x0F00) >> 8)
            << "].\n";

  delay_timer = V[(opcode & 0x0F00) >> 8];

  PC += 2;
}

void chip8::OP_FX18() {
  std::cout << PC << " Set sound timer to V[" << ((opcode & 0x0F00) >> 8)
            << "].\n";

  sound_timer = V[(opcode & 0x0F00) >> 8];

  PC += 2;
}

void chip8::OP_FX1E() {
  std::cout << PC << " Set I to I + V[" << ((opcode & 0x0F00) >> 8) << "]\n";

  I += V[(opcode & 0x0F00) >> 8];
  PC += 2;
}

void chip8::OP_FX29() {
  std::cout << PC << " Set I to location of sprite corresponding to V["
            << ((opcode & 0x0F00) >> 8) << "]\n";

  I = HexToFontCharLoc[V[((opcode & 0x0F00) >> 8)]];
  PC += 2;
}

void chip8::OP_FX33() {
  std::cout << PC << " Store BCD of V[" << ((opcode & 0x0F00) >> 8)
            << "] representation at " << I << "..." << I + 2 << "\n";
  memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
  memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
  memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;

  PC += 2;
}

void chip8::OP_FX55() {
  std::cout << PC << " Copy registers V[0] through V["
            << ((opcode & 0x0F00) >> 8) << "] into memory starting at I: " << I
            << ".\n";
  unsigned char X = ((opcode & 0x0F00) >> 8);

  for (int i = 0; i < X; i++) {
    memory[I + i] = V[i];
  }

  PC += 2;
}

void chip8::OP_FX65() {
  std::cout << PC << " Copy memory from " << I << "..."
            << I + ((opcode & 0x0F00) >> 8) << " into V[0..."
            << ((opcode & 0x0F00) >> 8) << "]\n";
  unsigned char X = ((opcode & 0x0F00) >> 8);

  for (int i = 0; i < X; ++i) {
    V[i] = memory[i + I];
  }
  PC += 2;
}

void chip8::OP_UNHANDLED() {
  std::stringstream ss;
  ss << "Unhandled opcode: " << std::hex << opcode << "\n";
  throw std::runtime_error(ss.str());
}
