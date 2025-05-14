#pragma once

#include <cstddef>
#include <cstdint>
#include "memory.h"

class CPU{
private:
  //registers
  unsigned char V[16];
  uint16_t I, PC;
  unsigned char DT, ST, SP;
  
  uint16_t stack[16];

  Memory memory;

public:
  CPU();
  CPU(uint16_t start);
  void execute();
  void loadBytes(const unsigned char bytes[], size_t n);
  void loadFile(const char* filename);
  void displayState();
};