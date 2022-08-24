#pragma once

#include <algorithm>
#include <vector>

extern std::vector<unsigned char> memory;
extern std::vector<unsigned short> stack;
extern std::vector<std::vector<unsigned char>> framebuffer;

extern std::vector<unsigned char> V;
extern unsigned short I, PC, DT, ST;

void init();
