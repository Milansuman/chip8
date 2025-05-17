#include "cpu.h"
#include "memory.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <random>

CPU::CPU() {
  // clearing the registers and stack
  for (int i = 0; i < 16; i++) {
    this->V[i] = 0;
    this->stack[i] = 0;
  }
  this->I = 0;
  this->DT = 0;
  this->ST = 0;

  this->memory = Memory();
  this->PC = 0x200; // CHIP8 programs usually start at 0x200

  this->keyInt = false;
  this->pressedKey = sf::Keyboard::Key::Unknown;
  this->keyCodeRegister = 0;
}

CPU::CPU(uint16_t start) : CPU() { this->PC = start; }

CPU::CPU(std::shared_ptr<std::vector<unsigned char>> vmem) : CPU() {
  this->vmem = vmem;
}

void CPU::loadBytes(const unsigned char bytes[], size_t n) {
  this->memory.loadBytes(bytes, this->PC, n);
}

void CPU::loadFile(const char *filename) {
  std::ifstream program;
  program.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

  char *memblock;
  size_t size;
  if (program.is_open()) {
    size = program.tellg();

    memblock = new char[size];
    program.seekg(0);
    program.read(memblock, size);
    program.close();

    this->memory.loadBytes((unsigned char *)memblock, this->PC, size);
    delete memblock;
  }
}

sf::Keyboard::Key CPU::getKeyCode(unsigned char value) {
  switch (value) {
  case 0x0:
    return sf::Keyboard::Key::Num0;
  case 0x1:
    return sf::Keyboard::Key::Num1;
  case 0x2:
    return sf::Keyboard::Key::Num2;
  case 0x3:
    return sf::Keyboard::Key::Num3;
  case 0x4:
    return sf::Keyboard::Key::Num4;
  case 0x5:
    return sf::Keyboard::Key::Num5;
  case 0x6:
    return sf::Keyboard::Key::Num6;
  case 0x7:
    return sf::Keyboard::Key::Num7;
  case 0x8:
    return sf::Keyboard::Key::Num8;
  case 0x9:
    return sf::Keyboard::Key::Num9;
  case 0xA:
    return sf::Keyboard::Key::A;
  case 0xB:
    return sf::Keyboard::Key::B;
  case 0xC:
    return sf::Keyboard::Key::C;
  case 0xD:
    return sf::Keyboard::Key::D;
  case 0xE:
    return sf::Keyboard::Key::E;
  case 0xF:
    return sf::Keyboard::Key::F;
  default:
    return sf::Keyboard::Key::Unknown;
  }
}

unsigned char CPU::getByte(sf::Keyboard::Key keycode){
  switch (keycode) {
  case sf::Keyboard::Key::Num0:
    return 0x0;
  case sf::Keyboard::Key::Num1:
    return 0x1;
  case sf::Keyboard::Key::Num2:
    return 0x2;
  case sf::Keyboard::Key::Num3:
    return 0x3;
  case sf::Keyboard::Key::Num4:
    return 0x4;
  case sf::Keyboard::Key::Num5:
    return 0x5;
  case sf::Keyboard::Key::Num6:
    return 0x6;
  case sf::Keyboard::Key::Num7:
    return 0x7;
  case sf::Keyboard::Key::Num8:
    return 0x8;
  case sf::Keyboard::Key::Num9:
    return 0x9;
  case sf::Keyboard::Key::A:
    return 0xA;
  case sf::Keyboard::Key::B:
    return 0xB;
  case sf::Keyboard::Key::C:
    return 0xC;
  case sf::Keyboard::Key::D:
    return 0xD;
  case sf::Keyboard::Key::E:
    return 0xE;
  case sf::Keyboard::Key::F:
    return 0xF;
  default:
    return 0xFF;
  }
}

void CPU::displayState() {
  std::cout << "CPU State:" << std::endl;

  // Display registers V0-VF
  std::cout << "Registers: \n";
  for (int i = 0; i < 16; i++) {
    std::cout << "V" << std::hex << i << ": 0x" << std::setw(2)
              << std::setfill('0') << static_cast<int>(this->V[i]) << " ";
  }
  std::cout << std::endl;

  // Display other registers
  std::cout << "I: 0x" << std::hex << std::setw(4) << std::setfill('0')
            << this->I << std::endl;
  std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0')
            << this->PC << std::endl;
  std::cout << "SP: " << std::dec << static_cast<int>(this->SP) << std::endl;

  // Display stack contents
  std::cout << "Stack: \n";
  for (int i = 0; i < 16; i++) {
    std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0')
              << this->stack[i] << " ";
  }
  std::cout << std::endl;

  // Display timers
  std::cout << "DT: " << std::dec << static_cast<int>(this->DT) << std::endl;
  std::cout << "ST: " << std::dec << static_cast<int>(this->ST) << std::endl;
}

void CPU::execute() {
  while (this->PC <= 0xFFF) {
    this->tick();
  }
}

void CPU::tick() {
  if (this->PC >= 0xFFF) {
    return;
  }

  if(this->keyInt){
    if(this->pressedKey != sf::Keyboard::Key::Unknown && this->getByte(this->pressedKey) != 0xFF){
      this->V[this->keyCodeRegister] = this->getByte(this->pressedKey);
      this->keyInt = false;
      this->pressedKey = sf::Keyboard::Key::Unknown;
      this->keyCodeRegister = 0;
    }else{
      return;
    }
  }

  unsigned char MSB = this->memory.getByte(this->PC);
  unsigned char LSB = this->memory.getByte(this->PC + 1);
  switch (MSB >> 4) {
  case 0x0:
    if (LSB == 0xE0) {
      // CLS
      for(size_t i=0; i<this->vmem->size(); i++){
        this->vmem->at(i) = 0;
      }
    } else if (LSB == 0xEE) {
      // RET
      this->PC = this->stack[this->SP];
      this->SP--;
    }
    break;
  case 0x1: {
    // JP address
    uint16_t address = ((MSB << 8) | LSB) ^ 0x1000;
    this->PC = address;
    return;
  }
  case 0x2: {
    // CALL address
    uint16_t address = ((MSB << 8) | LSB) ^ 0x2000;
    this->SP++;
    this->stack[this->SP] = this->PC;
    this->PC = address;
    return;
  }
  case 0x3: {
    // SE Vx, byte
    unsigned char pos = MSB ^ 0x30;
    if (this->V[pos] == LSB) {
      this->PC += 4;
      return;
    }
    break;
  }
  case 0x4: {
    // SNE Vx, byte
    unsigned char pos = MSB ^ 0x40;
    if (this->V[pos] != LSB) {
      this->PC += 4;
      return;
    }
    break;
  }
  case 0x5: {
    // SE Vx, Vy
    unsigned char x = MSB ^ 0x50;
    unsigned char y = LSB >> 4;
    if (this->V[x] == this->V[y]) {
      this->PC += 4;
      return;
    }
    break;
  }
  case 0x6: {
    // LD Vx, byte
    unsigned char pos = MSB ^ 0x60;
    this->V[pos] = LSB;
    break;
  }
  case 0x7: {
    // ADD Vx, byte
    unsigned char pos = MSB ^ 0x70;
    this->V[pos] += LSB;
    break;
  }
  case 0x8: {
    unsigned char x = MSB ^ 0x80;
    unsigned char y = LSB >> 4;
    unsigned char option = LSB ^ (y << 4);
    switch (option) {
    case 0x0:
      // LD Vx, Vy
      this->V[x] = this->V[y];
      break;
    case 0x1:
      // OR Vx, Vy
      this->V[x] |= this->V[y];
      break;
    case 0x2:
      // AND Vx, Vy
      this->V[x] &= this->V[y];
      break;
    case 0x3:
      // XOR Vx, Vy
      this->V[x] ^= this->V[y];
      break;
    case 0x4: {
      // ADD Vx, Vy
      uint16_t sum = this->V[x] + this->V[y];
      this->V[x] = sum & 0xFF;
      if (sum > 0xFF) {
        this->V[0xF] = 1;
      } else {
        this->V[0xF] = 0;
      }
      break;
    }
    case 0x5:
      // SUB Vx, Vy
      if (this->V[x] > this->V[y]) {
        this->V[0xF] = 1;
      } else {
        this->V[0xF] = 0;
      }
      this->V[x] -= this->V[y];
      break;
    case 0x6:
      // SHR Vx {, Vy}
      if (this->V[x] & 0x1) {
        this->V[0xF] = 1;
      } else {
        this->V[0xF] = 0;
      }
      this->V[x] /= 2;
      break;
    case 0x7:
      // SUBN Vx, Vy
      if (this->V[x] < this->V[y]) {
        this->V[0xF] = 1;
      } else {
        this->V[0xF] = 0;
      }
      this->V[x] = this->V[y] - this->V[x];
      break;
    case 0xE:
      // SHL Vx {, Vy}
      if (this->V[x] >> 7) {
        this->V[0xF] = 1;
      } else {
        this->V[0xF] = 0;
      }
      this->V[x] *= 2;
      break;
    }
    break;
  }
  case 0x9: {
    // SNE Vx, Vy
    unsigned char x = MSB ^ 0x90;
    unsigned char y = LSB >> 4;
    if (this->V[x] != this->V[y]) {
      this->PC += 4;
      return;
    }
    break;
  }
  case 0xA: {
    // LD I, addr
    uint16_t address = ((MSB << 8) | LSB) ^ 0xA000;
    this->I = address;
    break;
  }
  case 0xB: {
    // JP V0, addr
    uint16_t address = ((MSB << 8) | LSB) ^ 0xB000;
    this->PC = this->V[0] + address;
    return;
  }
  case 0xC: {
    // RND Vx, byte
    unsigned char x = MSB ^ 0xC0;
    std::random_device seedgen;
    std::mt19937 gen(seedgen());

    std::uniform_int_distribution<> distrib(0, 255);
    this->V[x] = LSB & distrib(gen);
    break;
  }
  case 0xD: {
    // DRW Vx, Vy, nibble
    unsigned char x = MSB ^ 0xD0;
    unsigned char y = LSB >> 4;
    unsigned char n = LSB & 0xF;

    for (int i = 0; i < n; i++) {
      unsigned char spriteByte = this->memory.getByte(this->I + i);

      int xBytePos = (this->V[x] % 64) / 8;
      int xBitPos = (this->V[x] % 64) % 8;

      int yPos = (this->V[y] + i) % 32;
      this->vmem->at(8 * yPos + xBytePos) ^= spriteByte >> xBitPos;
      this->vmem->at(8 * yPos + xBytePos + 1) ^= spriteByte << (8 - xBitPos);
    }

    break;
  }
  case 0xE: {
    unsigned char x = MSB ^ 0xE0;
    if (LSB == 0x9E) {
      // SKP Vx
      if (sf::Keyboard::isKeyPressed(getKeyCode(this->V[x]))) {
        PC += 4;
        return;
      }
    } else if (LSB == 0xA1) {
      // SKNP Vx
      if (!sf::Keyboard::isKeyPressed(getKeyCode(this->V[x]))) {
        PC += 4;
        return;
      }
    }
    break;
  }
  case 0xF: {
    unsigned char x = MSB ^ 0xF0;
    switch (LSB) {
    case 0x07:
      // LD Vx, DT
      this->V[x] = this->DT;
      break;
    case 0x0A:
      // LD Vx, K
      this->keyCodeRegister = x;
      this->keyInt = true;
      break;
    case 0x15:
      // LD DT, Vx
      this->DT = this->V[x];
      break;
    case 0x18:
      // LD ST, Vx
      this->ST = this->V[x];
      break;
    case 0x1E:
      // ADD I, Vx
      this->I += this->V[x];
      break;
    case 0x29:
      // LD F, Vx
      if (this->V[x] <= 0xF) {
        this->I = this->V[x] * 5;
      }
      break;
    case 0x33: {
      // LD B, Vx
      unsigned char value = this->V[x];
      this->memory.setByte(this->I + 0, value / 100);
      this->memory.setByte(this->I + 1, (value / 10) % 10);
      this->memory.setByte(this->I + 2, value % 10);
      break;
    }
    case 0x55:
      // LD [I], Vx
      for (int i = 0; i <= x; i++) {
        this->memory.setByte(this->I + i, this->V[i]);
      }
      break;
    case 0x65:
      // LD Vx, [I]
      for (int i = 0; i <= x; i++) {
        this->V[i] = this->memory.getByte(this->I + i);
      }
      break;
    }
  }
  }
  this->PC += 2;
}