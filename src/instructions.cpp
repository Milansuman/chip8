#include <algorithm>
#include "instructions.hpp"
#include "hardware.hpp"

void prog::sys(unsigned short addr){
    stack.push_back(PC);
    PC = addr;
}

void prog::cls(){
    framebuffer.clear();
}

void prog::ret(){
    PC = stack.pop_back();
}

void prog::jmp(unsigned short addr){
    PC = addr;
}

void prog::call(unsigned short addr){
    prog::sys(addr);
}

void prog::seByte(int reg, unsigned char byte){
    if(V[reg] == byte){
        PC += 2;
    }
}

void prog::sneByte(int reg, unsigned char byte){
    if(V[reg] != byte){
        PC += 2;
    }
}