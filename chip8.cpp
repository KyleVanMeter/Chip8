#include "chip8.h"
#include <iostream>
#include <sstream>
#include <string.h>

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
}

void chip8::load() {}

/*
 * Fetch, decode, execute opcodes, and update timers
 */
void chip8::emuCycle() {
  // opcode = memory[PC] << 8 | memory[PC + 1];

  for (const auto i : Opcodes::All) {
    opcode = i;

    std::cout << "Trying opcode: " << std::hex << opcode << " ";

    switch (opcode & 0xF000) {
    case 0x0000:
      switch (opcode & 0x00FF) {
      case Opcodes::Chip8::OP_0NNN:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_001N:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_00E0:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_00EE:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_00FA:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      default:
        std::cerr << "Unknown opcode: " << std::hex << opcode << "\n";
      }
      break;
    case Opcodes::Chip8::OP_1NNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_2NNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_3XNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_4XNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_5XY0:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_6XNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_7XNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case 0x8000:
      switch (opcode & 0xF00F) {
      case Opcodes::Chip8::OP_8XY0:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XY1:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XY2:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XY3:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XY4:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_8XY5:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
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
      }
      break;
    case Opcodes::Chip8::OP_9XY0:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_ANNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_BNNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_CXNN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case Opcodes::Chip8::OP_DXYN:
      std::cout << "Hit opcode " << std::hex << opcode << "\n";
      break;
    case 0xE000:
      switch (opcode & 0xF00F) {
      case Opcodes::Chip8::OP_EX9E:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_EXA1:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      default:
        std::cerr << "Unknown opcode in E: " << std::hex << opcode << "\n";
      }
      break;
    case 0xF000:
      switch (opcode & 0xF00F) {
      case Opcodes::Chip8::OP_FX07:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX0A:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX15:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX18:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX1E:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX29:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX33:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX55:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      case Opcodes::Chip8::OP_FX65:
        std::cout << "Hit opcode " << std::hex << opcode << "\n";
        break;
      default:
        std::cerr << "Unknown opcode in F: " << std::hex << opcode << "\n";
      }
      break;
    default:
      std::cerr << "Unknown opcode in default: " << std::hex << opcode << "\n";
    }
  }

  std::exit(0);
}
/*
switch (opcode & 0xF000) {
case 0x0000:
  switch (opcode & 0x000F) {
  case 0x0000: // Clear screen
    memset(gfx, 0, sizeof(gfx));
    PC += 2;
    break;
  case 0x000E: // Return from subroutine
    PC = stack[SP];
    SP--;
    break;
  }
  // Opcodes distinguished by first bit
case 0x1000: // Set PC to NNN
  PC = opcode & 0x0FFF;
  break;
case 0x2000: // Call subroutine at NNN
  stack[++SP] = PC;
  PC = opcode & 0x0FFF;
  break;
case 0x3000: // Skip next instruction if VX == NN
  PC += 2;
  if (V[opcode & 0x0F00] == (opcode & 0x00FF)) {
    PC += 2;
  }
  break;
case 0x4000: // Skip next instruction if VX !== NN
  PC += 2;
  if (V[opcode & 0x0F00] !== (opcode & 0x00FF)) {
    PC += 2;
  }
  break;
  case 0x
case 0xA000: // Set I to NNN
  I = opcode & 0x0FFF;
  PC += 2;
  break;
case 0xB000: // Set PC to NNN + V0

  break;
case 0xF01E: // Adds Vn to I
  I += V[opcode & 0x0F00];
  PC += 2;
  break;
case 0xF029: // Sets I to sprite location at Vn
  I = gfx[V[opcode & 0x0F00]];
  PC += 2;
  break;
case 0XF055: // Stores V0 to Vn in memory (I unaffected)
  for (int i = 0; i <= (opcode & 0x0F00); i++) {
    memory[I + i] = V[i];
  }
  PC += 2;
  break;
case 0xF065: // Fills V0 to Vn from memory (I unaffected)
  for (int i = 0; i <= (opcode & 0x0F00); i++) {
    V[i] = memory[I + i];
  }
  PC += 2;
  break;
}
*/

void chip8::setKeys() {}
