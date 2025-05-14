#pragma once

#include <cstddef>
#include <vector>

class Memory {
private:
  std::vector<unsigned char> memory;

public:
  Memory();
  unsigned char getByte(size_t pos);
  void setByte(size_t pos, unsigned char value);
  void loadBytes(const unsigned char* bytes, size_t pos, size_t n);
};