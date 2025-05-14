#include "cpu.h"
#include "memory.h"
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <random>
#include <fstream>
#include <iostream>

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
}

CPU::CPU(uint16_t start) : CPU() { this->PC = start; }

void CPU::loadBytes(const unsigned char bytes[], size_t n){
  this->memory.loadBytes(bytes, this->PC, n);
}

void CPU::loadFile(const char* filename){
  std::ifstream program;
  program.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

  char* memblock;
  size_t size;
  if(program.is_open()){
    size = program.tellg();

    memblock = new char[size];
    program.seekg(0);
    program.read(memblock, size);
    program.close();

    this->memory.loadBytes((unsigned char*)memblock, this->PC, size);
    delete memblock;
  }
}

void CPU::displayState(){
  std::cout << "CPU State:" << std::endl;

  // Display registers V0-VF
  std::cout << "Registers: \n";
  for (int i = 0; i < 16; i++) {
    std::cout << "V" << std::hex << i << ": 0x" << std::setw(2) << std::setfill('0')
              << static_cast<int>(this->V[i]) << " ";
  }
  std::cout << std::endl;

  // Display other registers
  std::cout << "I: 0x" << std::hex << std::setw(4) << std::setfill('0') << this->I << std::endl;
  std::cout << "PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << this->PC << std::endl;
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
  std::cout << "ST: " << std::dec << static_cast<int>(this->ST) << "\n" << std::endl;
}

void CPU::execute() {
  while (this->PC <= 0xFFF) {
    unsigned char MSB = this->memory.getByte(this->PC);
    unsigned char LSB = this->memory.getByte(this->PC + 1);
    switch (MSB >> 4) {
    case 0x0:
      if(LSB == 0xE0){
        //CLS
      }else if(LSB == 0xEE){
        // RET
        this->PC = this->stack[this->SP];
        this->SP--;
      }
      break;
    case 0x1: {
      // JP address
      uint16_t address = ((MSB << 8) | LSB) ^ 0x1000;
      this->PC = address;
      break;
    }
    case 0x2: {
      // CALL address
      uint16_t address = ((MSB << 8) | LSB) ^ 0x2000;
      this->SP++;
      this->stack[this->SP] = this->PC;
      this->PC = address;
      break;
    }
    case 0x3: {
      // SE Vx, byte
      unsigned char pos = MSB ^ 0x30;
      if(this->V[pos] == LSB){
        this->PC += 2;
        continue;
      }
      break;
    }
    case 0x4: {
      // SNE Vx, byte
      unsigned char pos = MSB ^ 0x40;
      if(this->V[pos] != LSB){
        this->PC += 4;
        continue;
      }
      break;
    }
    case 0x5: {
      // SE Vx, Vy
      unsigned char x = MSB ^ 0x50;
      unsigned char y = LSB >> 4;
      if(this->V[x] == this->V[y]){
        this->PC += 4;
        continue;
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
      case 0x4:{
        // ADD Vx, Vy
        uint16_t sum = this->V[x] + this->V[y];
        this->V[x] = sum & 0xFF;
        if(sum > 0xFF){
          this->V[0xF] = 1;
        }else{
          this->V[0xF] = 0;
        }
        break;
      }
      case 0x5:
        // SUB Vx, Vy
        if(this->V[x] > this->V[y]){
          this->V[0xF] = 1;
        }else{
          this->V[0xF] = 0;
        }
        this->V[x] -= this->V[y];
        break;
      case 0x6:
        // SHR Vx {, Vy}
        if(this->V[x] & 0x1){
          this->V[0xF] = 1;
        }else{
          this->V[0xF] = 0;
        }
        this->V[x] /= 2;
        break;
      case 0x7:
        // SUBN Vx, Vy
        if(this->V[x] < this->V[y]){
          this->V[0xF] = 1;
        }else{
          this->V[0xF] = 0;
        }
        this->V[x] = this->V[y] - this->V[y];
        break;
      case 0xE:
        // SHL Vx {, Vy}
        if(this->V[x] >> 7){
          this->V[0xF] = 1;
        }else{
          this->V[0xF] = 0;
        }
        this->V[x] *= 2;
        break;
      }
      break;
    }
    case 0x9: {
      // SNE Vx, Vy
      unsigned char x = MSB ^ 0x80;
      unsigned char y = LSB >> 4;
      if(this->V[x] != this->V[y]){
        this->PC += 4;
        continue;
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
      uint16_t address = ((MSB << 8) | LSB) ^ 0xA000;
      this->PC = this->V[0] + address;
      continue;
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

      unsigned char sprite[n];
      for(int i=0; i<n; i++){
        sprite[i] = this->memory.getByte(this->I+i);
      }

      // handle display
      break;
    }
    case 0xE: {
      unsigned char x = MSB ^ 0xD0;
      if(LSB == 0x9E){
        // SKP Vx
      }else if(LSB == 0xA1){
        // SKNP Vx
      }
      break;
    }
    case 0xF: {
      unsigned char x = MSB ^ 0xD0;
      switch (LSB) {
      case 0x07:
        // LD Vx, DT
        this->V[x] = this->DT;
        break;
      case 0x0A:
        // LD Vx, K
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
        if(this->V[x] <= 0xF){
          this->I = this->V[x] * 5;
        }
        break;
      case 0x33: {
        // LD B, Vx
        unsigned char byte = this->V[x];
        for(int i=0; i<3; i++){
          this->memory.setByte(this->I+i, byte%10);
          byte /= 10;
        }
        break;
      }
      case 0x55:
        // LD [I], Vx
        for(int i=0; i<x; i++){
          this->memory.setByte(this->I+i, this->V[i]);
        }
        break;
      case 0x65:
        // LD Vx, [I]
        for(int i=0; i<x; i++){
          this->V[i] = this->memory.getByte(this->I+i);
        }
        break;
      }
    }
    }
    this->PC += 2;
  }
}