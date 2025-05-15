#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "memory.h"

class CPU{
private:
  //registers - V0 to VF are general purpose 8 bit registers
  unsigned char V[16];
  uint16_t I, PC;
  unsigned char DT, ST, SP;
  uint16_t stack[16];
  Memory memory;
  std::shared_ptr<std::vector<unsigned char>> vmem;

public:
  CPU();
  CPU(uint16_t start);
  CPU(std::shared_ptr<std::vector<unsigned char>> vmem);
  
  void execute();
  //using tick method to take advantage of display framerate directly
  //sounds a bit weird when I say it out loud, but ig it's fine for a simple emulator
  void tick();
  void loadBytes(const unsigned char bytes[], size_t n);
  void loadFile(const char* filename);
  void displayState();
};