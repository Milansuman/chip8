#pragma once

#include <vector>

namespace chip8{
    class hardware{
        std::vector<unsigned char> memory;
        std::vector<std::vector<unsigned char>> framebuffer;
        std::vector<unsigned short> stack;
        std::vector<unsigned char> V;
        unsigned short I, PC;
        unsigned char DT, ST;

        public:
        hardware();
        void cls();
        void ret();
        void jmp(unsigned short addr);
        void call(unsigned short addr);
        void seByte(int reg, unsigned char byte);
        void sneByte(int reg, unsigned char byte);
        void seReg(int reg1, int reg2);
        void ldByte(int reg, unsigned char byte);
        void addByte(int reg, unsigned char byte);
        void ldReg(int reg1, int reg2);
        void orReg(int reg1, int reg2);
        void andReg(int reg1, int reg2);
        void xorReg(int reg1, int reg2);
        void addReg(int reg1, int reg2);
        void subReg(int reg1, int reg2);
        void shr(int reg);
        void subnReg(int reg1, int reg2);
        void shl(int reg);
        void sneReg(int reg1, int reg2);
        void ldI(unsigned char addr);
        void jpReg(unsigned char addr);
        void rnd(int reg, unsigned char byte);
        void drw(int reg1, int reg2, unsigned char nibble);
        void skp(int reg);
        void sknp(int reg);
        void ldDT(int reg);
        void ldK(int reg);
        void ldDTReg(int reg);
        void ldSTReg(int reg);
        void addIReg(int reg);
        void ldDigit(int reg);
        void ldBCD(int reg);
        void ldWriteReg(int reg);
        void ldReadReg(int reg);
    };
}