#include "chip8.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#define OP_DEBUG 0

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
#if OP_DEBUG != 1
  opcode = memory[PC] << 8 | memory[PC + 1];
#endif

#if OP_DEBUG == 1
  //  for (const auto i : Opcodes::All) {
  //    opcode = i;
  for (int i = 0x200; i < 4096 - 0x200; i += 2) {
    opcode = memory[i] << 8 | memory[i + 1];

    std::cout << "Trying opcode: " << std::hex << opcode << " ";
#endif

    switch (opcode & 0xF000) {
    case 0x0000:
      switch (opcode & 0x00FF) {
      case Opcodes::Chip8::OP_0NNN:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        PC += 2;

        std::exit(0);

        break;
      case Opcodes::Chip8::OP_001N:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_00E0:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
      // std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
      // std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        PC += 2;
        break;
      case Opcodes::Chip8::OP_8XY1:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XY2:
        // std::cout << "Hit opcode " << std::hex << opcode << "\n";
        std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8) << "] to "
                  << " V[" << ((opcode & 0x0F00) >> 8) << "] & V["
                  << ((opcode & 0x00F0) >> 4) << "]\n";

        V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
        PC += 2;
        break;
      case Opcodes::Chip8::OP_8XY3:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        PC += 2;
        break;
      case Opcodes::Chip8::OP_8XY6:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XY7:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XYE:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      default:
        std::cerr << "Unknown opcode: " << std::hex << opcode << "\n";
        std::exit(-1);
      }
      break;
    case Opcodes::Chip8::OP_9XY0:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";

      PC += 2;
      break;
    case Opcodes::Chip8::OP_ANNN:
      // Set I to NNN
      std::cout << PC << " Set I to " << (opcode & 0x0FFF) << "\n";
      I = opcode & 0x0FFF;
      PC += 2;
      break;
    case Opcodes::Chip8::OP_BNNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_CXNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      PC += 2;
      break;
    case Opcodes::Chip8::OP_DXYN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      PC += 2;
      break;
    case 0xE000:
      switch (opcode & 0xF00F) {
      case Opcodes::Chip8::OP_EX9E:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_EXA1:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        PC += 2;
        break;
      default:
        std::cerr << "Unknown opcode in E: " << std::hex << opcode << "\n";
        std::exit(-1);
      }
      break;
    case 0xF000:
      switch (opcode & 0xF00F) {
      case Opcodes::Chip8::OP_FX07:
        // std::cout << "Hit opcode " << std::hex << opcode << "\n";
        std::cout << PC << " Set V[" << ((opcode & 0x0F00) >> 8)
                  << "] to delay_timer (" << delay_timer << ")\n";

        V[(opcode & 0x0F00) >> 8] = delay_timer;
        PC += 2;
        break;
      case Opcodes::Chip8::OP_FX0A:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX18:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        PC += 2;
        break;
      case Opcodes::Chip8::OP_FX1E:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
          std::cout << "Hit opcode " << std::hex << opcode << "\n";
          PC += 2;
          break;
        case Opcodes::Chip8::OP_FX55:
          std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
#if OP_DEBUG == 1
  }

  std::exit(0);
#endif

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

void chip8::setKeys() {}
