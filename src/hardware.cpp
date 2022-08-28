#include <vector>
#include <random>
#include "hardware.hpp"

namespace chip8{
    hardware::hardware()
        : memory(std::vector<unsigned char>(4096, 0)), 
          framebuffer(std::vector<std::vector<unsigned char>>(64, std::vector<unsigned char>(32))), 
          stack(std::vector<unsigned short>(16, 0)), 
          V(std::vector<unsigned char>(16, 0)),
          I(0), PC(0), DT(0), ST(0) 
    {
        this->memory.insert(this->memory.begin(), {0xF0, 0x90, 0x90, 0x90, 0xF0});
        this->memory.insert(this->memory.begin()+5, {0x20, 0x60, 0x20, 0x20, 0x70});
        this->memory.insert(this->memory.begin()+10, {0xF0, 0x10, 0xF0, 0x80, 0xF0});
        this->memory.insert(this->memory.begin()+15, {0xF0, 0x10, 0xF0, 0x10, 0xF0});
        this->memory.insert(this->memory.begin()+20, {0x90, 0x90, 0xF0, 0x10, 0x10});
        this->memory.insert(this->memory.begin()+25, {0xF0, 0x80, 0xF0, 0x10, 0xF0});
        this->memory.insert(this->memory.begin()+30, {0xF0, 0x80, 0xF0, 0x90, 0xF0});
        this->memory.insert(this->memory.begin()+35, {0xF0, 0x10, 0x20, 0x40, 0x40});
        this->memory.insert(this->memory.begin()+40, {0xF0, 0x90, 0xF0, 0x90, 0xF0});
        this->memory.insert(this->memory.begin()+45, {0xF0, 0x90, 0xF0, 0x10, 0xF0});
        this->memory.insert(this->memory.begin()+50, {0xF0, 0x90, 0xF0, 0x90, 0x90});
        this->memory.insert(this->memory.begin()+55, {0xE0, 0x90, 0xE0, 0x90, 0xE0});
        this->memory.insert(this->memory.begin()+60, {0xF0, 0x80, 0x80, 0x80, 0xF0});
        this->memory.insert(this->memory.begin()+65, {0xE0, 0x90, 0x90, 0x90, 0xE0});
        this->memory.insert(this->memory.begin()+70, {0xF0, 0x80, 0xF0, 0x80, 0xF0});
        this->memory.insert(this->memory.begin()+75, {0xF0, 0x80, 0xF0, 0x80, 0x80});
        this->memory.resize(4096);
    }

    void hardware::cls(){
        this->framebuffer.clear();
    }

    void hardware::ret(){
        this->PC = this->stack[this->stack.size()];
        this->stack.pop_back();
    }

    void hardware::jmp(unsigned short addr){
        this->PC = addr;
    }

    void hardware::call(unsigned short addr){
        this->stack.push_back(this->PC);
        this->PC = addr;
    }

    void hardware::seByte(int reg, unsigned char byte){
        if(this->V[reg] == byte){
            this->PC += 2;
        }
    }

    void hardware::sneByte(int reg, unsigned char byte){
        if(this->V[reg] != byte){
            this->PC += 2;
        }
    }

    void hardware::seReg(int reg1, int reg2){
        if(this->V[reg1] == this->V[reg2]){
            this->PC += 2;
        }
    }

    void hardware::ldByte(int reg, unsigned char byte){
        this->V[reg] = byte;
    }

    void hardware::addByte(int reg, unsigned char byte){
        this->V[reg] += byte;
    }

    void hardware::ldReg(int reg1, int reg2){
        this->V[reg1] = this->V[reg2];
    }

    void hardware::orReg(int reg1, int reg2){
        this->V[reg1] |= this->V[reg2];
    }

    void hardware::andReg(int reg1, int reg2){
        this->V[reg1] &= this->V[reg2];
    }

    void hardware::xorReg(int reg1, int reg2){
        this->V[reg1] ^= this->V[reg2];
    }

    void hardware::addReg(int reg1, int reg2){
        if(this->V[reg1] + this->V[reg2] > 0xFF){
            this->V[0xF] = 1;
        }else{
            this->V[0xF] = 0; // vF could've been set before
        }
        this->V[reg1] += this->V[reg2];
    }

    void hardware::subReg(int reg1, int reg2){
        if(this->V[reg1] > this->V[reg2]){
            this->V[0xF] = 1;
        }else{
            this->V[0xF] = 0; // vF could've been set before
        }
        this->V[reg1] -= this->V[reg2];
    }

    void hardware::shr(int reg){
        if(this->V[reg] & (1 == 1)){
            this->V[0xF] = 1;
        }else{
            this->V[0xF] = 0; // vF could've been set before
        }
        this->V[reg] /= 2;
    }

    void hardware::subnReg(int reg1, int reg2){
        if(this->V[reg2] > this->V[reg1]){
            this->V[0xF] = 1;
        }else{
            this->V[0xF] = 0; // vF could've been set before
        }
        this->V[reg1] = this->V[reg2] - this->V[reg1];
    }

    void hardware::shl(int reg){
        if(this->V[reg] & (0x80 == 1)){ // 0x80 == 1000 0000. We do this to get the most significant bit
            this->V[0xF] = 1;
        }else{
            this->V[0xF] = 0; // vF could've been set before
        }
        this->V[reg] *= 2;
    }

    void hardware::sneReg(int reg1, int reg2){
        if(this->V[reg1] != this->V[reg2]){
            this->PC += 2;
        }
    }

    void hardware::ldI(unsigned char addr){
        this->I = addr;
    }

    void hardware::jpReg(unsigned char addr){
        this->PC = this->V[0x0] + addr;
    }

    void hardware::rnd(int reg, unsigned char byte){
        std::default_random_engine generator;
        std::uniform_int_distribution<unsigned char> distribution(0x00, 0xFF);
        unsigned char randByte = distribution(generator);

        this->V[reg] = randByte & byte;
    }

    void hardware::drw(int reg1, int reg2, unsigned char byte){
        unsigned char nybble = byte&0xF;
        unsigned char x = this->V[reg1];
        unsigned char y = this->V[reg2];

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

            unsigned char frame = this->framebuffer[x][y]; //saving frame data so we can check for pixel erasure
            this->framebuffer[x][y] ^= this->memory[I+i];
            for(unsigned char i=0; i<sizeof(frame); i++){
                unsigned char oldBit = (frame>>i) & 1;
                unsigned char newBit = (this->framebuffer[x][y]>>i) & 1;
                if(oldBit == 1 && newBit == 0){ // checking if pixel was erased
                    this->V[0xF] = 1;
                    break;
                }
            }
            
            y++;    
        }
    }

    void hardware::skp(int reg){
        //TODO: implement
    }

    void hardware::sknp(int reg){
        //TODO: implement
    }

    void hardware::ldDT(int reg){
        this->V[reg] = this->DT;
    }

    void hardware::ldK(int reg){
        //TODO: implement
    }

    void hardware::ldDTReg(int reg){
        this->DT = this->V[reg];
    }

    void hardware::ldSTReg(int reg){
        this->ST = this->V[reg];
    }

    void hardware::addIReg(int reg){
        this->I += this->V[reg];
    }

    void hardware::ldDigit(int reg){
        this->I = this->V[reg]*5;
    }

    void hardware::ldBCD(int reg){
        unsigned char hundrends = std::floor(this->V[reg]/100);
        unsigned char tens = std::floor((this->V[reg] - hundrends*100)/10);
        unsigned char ones = std::floor((this->V[reg] - (hundrends*100 + tens*10)));

        memory[I] = hundrends;
        memory[I+1] = tens;
        memory[I+2] = ones;
    }

    void hardware::ldWriteReg(int reg){
        for(int i=0; i<=reg; i++){
            this->memory[this->I+i] = this->V[i];
        }
    }

    void hardware::ldReadReg(int reg){
        for(int i=0; i<=reg; i++){
            this->V[i] = this->memory[this->I+i];
        }
    }

}