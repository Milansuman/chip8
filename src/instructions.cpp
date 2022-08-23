#include <algorithm>
#include <random>
#include <vector>
#include <cmath>
#include "instructions.hpp"
#include "hardware.hpp"

//these functions will make writing the run function easier
void prog::sys(unsigned short addr){
    stack.push_back(PC);
    PC = addr;
}

void prog::cls(){
    framebuffer.clear();
}

void prog::ret(){
    PC = stack[stack.size()];
    stack.pop_back();
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

void prog::seReg(int reg1, int reg2){
    if(V[reg1] == V[reg2]){
        PC += 2;
    }
}

void prog::ldByte(int reg, unsigned char byte){
    V[reg] = byte;
}

void prog::addByte(int reg, unsigned char byte){
    V[reg] += byte;
}

void prog::ldReg(int reg1, int reg2){
    V[reg1] = V[reg2];
}

void prog::orReg(int reg1, int reg2){
    V[reg1] |= V[reg2];
}

void prog::andReg(int reg1, int reg2){
    V[reg1] &= V[reg2];
}

void prog::xorReg(int reg1, int reg2){
    V[reg1] ^= V[reg2];
}

void prog::addReg(int reg1, int reg2){
    if(V[reg1] + V[reg2] > 0xFF){
        V[0xF] = 1;
    }else{
        V[0xF] = 0; // vF could've been set before
    }
    V[reg1] += V[reg2];
}

void prog::subReg(int reg1, int reg2){
    if(V[reg1] > V[reg2]){
        V[0xF] = 1;
    }else{
        V[0xF] = 0; // vF could've been set before
    }
    V[reg1] -= V[reg2];
}

void prog::shr(int reg){
    if(V[reg] & (1 == 1)){
        V[0xF] = 1;
    }else{
        V[0xF] = 0; // vF could've been set before
    }
    V[reg] /= 2;
}

void prog::subnReg(int reg1, int reg2){
    if(V[reg2] > V[reg1]){
        V[0xF] = 1;
    }else{
        V[0xF] = 0; // vF could've been set before
    }
    V[reg1] = V[reg2] - V[reg1];
}

void prog::shl(int reg){
    if(V[reg] & (0x80 == 1)){ // 0x80 == 1000 0000. We do this to get the most significant bit
        V[0xF] = 1;
    }else{
        V[0xF] = 0; // vF could've been set before
    }
    V[reg] *= 2;
}

void prog::sneReg(int reg1, int reg2){
    if(V[reg1] != V[reg2]){
        PC += 2;
    }
}

void prog::ldI(unsigned char addr){
    I = addr;
}

void prog::jpReg(unsigned char addr){
    PC = V[0x0] + addr;
}

void prog::rnd(int reg, unsigned char byte){
    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned char> distribution(0x00, 0xFF);
    unsigned char randByte = distribution(generator);

    V[reg] = randByte & byte;
}

void prog::drw(int reg1, int reg2, unsigned char byte){
    unsigned char nybble = byte&0xF;
    unsigned char x = V[reg1];
    unsigned char y = V[reg2];

    for(unsigned char i=0; i<nybble; i++){
        //out of bounds checking
        if(x > 64){
            x -= 64;
        }else if(x < 0){
            x += 64;
        }

        if(y > 32){
            y -= 32;
        }else if(y < 0){
            y += 32;
        }

        unsigned char frame = framebuffer[x][y]; //saving frame data so we can check for pixel erasure
        framebuffer[x][y] ^= memory[I+i];
        for(unsigned char i=0; i<sizeof(frame); i++){
            unsigned char oldBit = (frame>>i) & 1;
            unsigned char newBit = (framebuffer[x][y]>>i) & 1;
            if(oldBit == 1 && newBit == 0){ // checking if pixel was erased
                V[0xF] = 1;
                break;
            }
        }
        
        y++;    
    }
}

void prog::skp(int reg){
    //TODO: implement
}

void prog::sknp(int reg){
    //TODO: implement
}

void prog::ldDT(int reg){
    V[reg] = DT;
}

void prog::ldK(int reg){
    //TODO: implement
}

void prog::ldDTReg(int reg){
    DT = V[reg];
}

void prog::ldSTReg(int reg){
    ST = V[reg];
}

void prog::addIReg(int reg){
    I += V[reg];
}

void prog::ldDigit(int reg){
    I = V[reg]*5;
}

void prog::ldBCD(int reg){
    unsigned char hundrends = std::floor(V[reg]/100);
    unsigned char tens = std::floor((V[reg] - hundrends*100)/10);
    unsigned char ones = std::floor((V[reg] - (hundrends*100 + tens*10)));

    memory[I] = hundrends;
    memory[I+1] = tens;
    memory[I+2] = ones;
}

void prog::ldWriteReg(int reg){
    for(int i=0; i<=reg; i++){
        memory[I+i] = V[i];
    }
}

void prog::ldReadReg(int reg){
    for(int i=0; i<=reg; i++){
        V[i] = memory[I+i];
    }
}