#include "chip8.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string.h>

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
  // Program starts at 0x200, reset opcode, index, and stack pointer to 0
  PC = 0x200;
  opcode = 0;
  I = 0;
  SP = 0;

  // load fontset
  for (int i = 0; i < 80; ++i) {
    memory[i] = fontset[i];
  }

  // for (unsigned char i = 0; i < 15; ++i) {
  //  HexToFontCharLoc[i] = i * 5; // 5 is the row length for the fontset.
  //}
}

void chip8::load() {
  if (std::ifstream file{"PONG", std::ios::binary | std::ios::ate}) {
    char *block;
    std::streampos size = file.tellg();

    block = new char[size];

    file.seekg(0);
    if (file.read(&block[0], size)) {
      for (int i = 0; i < size; ++i) {
        memory[i + 0x200] = block[i];
      }
    } else {
      std::cerr << "broke" << std::endl;
    }

    delete[] block;
  } else {
    std::cerr << "Failed to open file PONG" << std::endl;
  }
}

/*
 * Fetch, decode, execute opcodes, and update timers
 */
void chip8::emuCycle() {
  std::default_random_engine gen;
  std::uniform_int_distribution<int> distribution(0, 255);

  opcode = memory[PC] << 8 | memory[PC + 1];

  switch (opcode & 0xF000) {
  case 0x0000:
    switch (opcode & 0x00FF) {
    case Opcodes::Chip8::OP_0NNN:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      PC += 2;

      std::exit(0);

      break;
    case Opcodes::Chip8::OP_001N:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_00E0:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_00EE:
      std::cout << PC << " Return from subroutine at " << stack[SP] << "\n";

      PC = stack[SP];
      --SP;
      PC += 2;
      break;
    case Opcodes::Chip8::OP_00FA:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    default:
      std::cerr << "Unknown opcode: " << std::hex << opcode << "\n";
      std::exit(-1);
    }
    break;
  case Opcodes::Chip8::OP_1NNN:
    std::cout << PC << " Jump to " << ((opcode & 0x0FFF)) << "\n";

    PC = opcode & 0x0FFF;
    break;
  case Opcodes::Chip8::OP_2NNN:
    std::cout << PC << " Call subroutine at " << (opcode & 0x0FFF) << "\n";

    ++SP;
    stack[SP] = PC;
    PC = (opcode & 0x0FFF);
    break;
  case Opcodes::Chip8::OP_3XNN:
    std::cout << PC << " Skip if V[" << ((opcode & 0x0F00) >> 8)
              << "] == " << (opcode & 0x00FF) << "\n";

    if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
      PC += 4;
    } else {
      PC += 2;
    }

    break;
  case Opcodes::Chip8::OP_4XNN:
    std::cout << PC << " Skip if V[" << ((opcode & 0x0F00) >> 8)
              << "] != " << (opcode & 0x00FF) << "\n";

    if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
      PC += 4;
    } else {
      PC += 2;
    }
    break;
  case Opcodes::Chip8::OP_5XY0:
    std::cerr << "Hit opcode " << std::hex << opcode << "\n";
    break;
  case Opcodes::Chip8::OP_6XNN:
    // Load immediate value NN into VX
    std::cout << PC << " Load immediate value " << (opcode & 0x00FF)
              << " into V[" << ((opcode & 0x0F00) >> 8) << "]\n";
    V[((opcode & 0x0F00) >> 8)] = (opcode & 0x00FF);
    PC += 2;
    break;
  case Opcodes::Chip8::OP_7XNN:
    std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to "
              << ((opcode & 0x00FF)) << " + V[" << ((opcode & 0x0F00) >> 8)
              << "]\n";

    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    PC += 2;
    break;
  case 0x8000:
    switch (opcode & 0xF00F) {
    case Opcodes::Chip8::OP_8XY0:
      std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] = V["
                << ((opcode & 0x00F0) >> 4) << "].";

      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
      PC += 2;
      break;
    case Opcodes::Chip8::OP_8XY1:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_8XY2:
      std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to "
                << " V[" << ((opcode & 0x0F00) >> 8) << "] & V["
                << ((opcode & 0x00F0) >> 4) << "]\n";

      V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
      PC += 2;
      break;
    case Opcodes::Chip8::OP_8XY3:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_8XY4: {
      std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to V["
                << ((opcode & 0x0F00) >> 8) << "] + V["
                << ((opcode & 0x00F0) >> 4) << "].  ";

      if (V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4] > 255) {
        std::cout << "V[f] = 1\n";
        V[0xF] = 1;
      } else {
        std::cout << "V[f] = 0\n";
        V[0xF] = 0;
      }
      V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
      PC += 2;
      break;
    }
    case Opcodes::Chip8::OP_8XY5:
      std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to V["
                << ((opcode & 0x0F00) >> 8) << "] - V["
                << ((opcode & 0x00F0) >> 4) << "].  ";

      if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
        std::cout << "V[f] = 1\n";
        V[0xF] = 1;
      } else {
        std::cout << "V[f] = 0\n";
        V[0xF] = 0;
      }

      V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
      PC += 2;
      break;
    case Opcodes::Chip8::OP_8XY6:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_8XY7:
      std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to V["
                << ((opcode & 0x00F0) >> 4) << "] - V["
                << ((opcode & 0x0F00) >> 8) << "].  ";

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
      break;
    case Opcodes::Chip8::OP_8XYE:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    default:
      std::cerr << "Unknown opcode: " << std::hex << opcode << "\n";
      std::exit(-1);
    }
    break;
  case Opcodes::Chip8::OP_9XY0:
    std::cerr << "Hit opcode " << std::hex << opcode << "\n";

    PC += 2;
    break;
  case Opcodes::Chip8::OP_ANNN:
    // Set I to NNN
    std::cout << PC << " Set I to " << (opcode & 0x0FFF) << "\n";
    I = opcode & 0x0FFF;
    PC += 2;
    break;
  case Opcodes::Chip8::OP_BNNN:
    std::cerr << "Hit opcode " << std::hex << opcode << "\n";
    break;
  case Opcodes::Chip8::OP_CXNN: {
    std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8)
              << "] = Random byte & " << ((opcode & 0x00FF) >> 4) << "\n";

    V[(opcode & 0x0F00) >> 8] = distribution(gen) & ((opcode & 0x00FF) >> 4);

    PC += 2;
    break;
  }
  case Opcodes::Chip8::OP_DXYN: {
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
          gfx[(x + xline + ((y + yline) * 64))] ^= 1;
        }
      }
    }

    drawFlag = true;
    PC += 2;
    break;
  }
  case 0xE000:
    switch (opcode & 0xF00F) {
    case Opcodes::Chip8::OP_EX9E:
      // Skip next instruction if V[X] is pressed
      std::cout << PC << " Skip next instruction if V["
                << ((opcode & 0x0F00) >> 8) << "] is pressed.";

      if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
        std::cout << "  Skipped!";
        PC += 4;
      } else {
        std::cout << "  Not skipped!";
        PC += 2;
      }
      std::cout << std::endl;

      break;
    case Opcodes::Chip8::OP_EXA1:
      // Skip next instruction if V[X] is pressed
      std::cout << PC << " Skip next instruction if V["
                << ((opcode & 0x0F00) >> 8) << "] is not pressed.";

      if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
        std::cout << "  Not skipped!";
        PC += 2;
      } else {
        std::cout << "  Skipped!";
        PC += 4;
      }
      std::cout << std::endl;

      break;
    default:
      std::cerr << "Unknown opcode in E: " << std::hex << opcode << "\n";
      std::exit(-1);
    }
    break;
  case 0xF000:
    switch (opcode & 0xF00F) {
    case Opcodes::Chip8::OP_FX07:
      std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8)
                << "] to delay_timer (" << delay_timer << ")\n";

      V[(opcode & 0x0F00) >> 8] = delay_timer;
      PC += 2;
      break;
    case Opcodes::Chip8::OP_FX0A:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_FX18:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      PC += 2;
      break;
    case Opcodes::Chip8::OP_FX1E:
      std::cerr << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_FX29:
      std::cout << PC << " Set I to location of sprite corresponding to V["
                << ((opcode & 0x0F00) >> 8) << "]\n";

      I = HexToFontCharLoc[V[((opcode & 0x0F00) >> 8)]];
      PC += 2;
      break;
    case Opcodes::Chip8::OP_FX33: {
      std::cout << PC << " Store BCD of V[" << ((opcode & 0x0F00) >> 8)
                << "] representation at " << I << "..." << I + 2 << "\n";
      memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
      memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
      memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;

      PC += 2;
      break;
    }
    case 0xF005:
      switch (opcode & 0xF0FF) {
      case Opcodes::Chip8::OP_FX15:
        std::cerr << "Hit opcode " << std::hex << opcode << "\n";
        PC += 2;
        break;
      case Opcodes::Chip8::OP_FX55:
        std::cerr << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX65: {
        std::cout << PC << " Copy memory from " << I << "..."
                  << I + ((opcode & 0x0F00) >> 8) << " into V[0..."
                  << ((opcode & 0x0F00) >> 8) << "]\n";
        unsigned char X = ((opcode & 0x0F00) >> 8);

        for (int i = 0; i < X; ++i) {
          V[i] = memory[i + I];
        }
        PC += 2;
        break;
      }
      default:
        std::cerr << "Unknown opcode in F005: " << std::hex << opcode << "\n";
        std::exit(-1);
      }
      break;
    default:
      std::cerr << "Unknown opcode in F: " << std::hex << opcode << "\n";
      std::exit(-1);
    }
    break;
  default:
    std::cerr << "Unknown opcode in default: " << std::hex << opcode << "\n";
    std::exit(-1);
  }

  if (delay_timer > 0) {
    --delay_timer;
  }

  if (sound_timer > 0) {
    if (sound_timer == 1) {
      std::cout << "BEEP!\n";
      std::exit(0);
    }

    --sound_timer;
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
        std::cerr << "Keydown: " << e.key.keysym.sym << "\n";
        key[KeySymToIndex[e.key.keysym.sym]] = 1;
      }
      break;
    case SDL_KEYUP:
      if (KeySymToIndex.find(e.key.keysym.sym) != KeySymToIndex.end()) {
        std::cerr << "Keyup: " << e.key.keysym.sym << "\n";
        key[KeySymToIndex[e.key.keysym.sym]] = 0;
      }
    default:
      std::cerr << "Unhandled event.\n";
    }
  }
}
