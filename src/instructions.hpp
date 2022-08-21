#pragma once

#include <algorithm>
#include "hardware.hpp"

enum INSTRUCTION{
    SYS,
    CLS,
    RET,
    JP,
    CALL,
    SE_BYTE,
    SNE_BYTE,
    SE_REG,
    LD_BYTE,
    ADD_BYTE,
    LD_REG,
    OR,
    AND,
    XOR,
    ADD_REG,
    SUB_REG,
    SHR,
    SUBN,
    SHL,
    SNE_REG,
    LD_I,
    JP_REG,
    RND,
    DRW,
    SKP,
    SKNP,
    LD_REGDT,
    LD_K,
    LD_DTREG,
    LD_STREG,
    ADD_IREG,
    LD_DIGIT,
    LD_BCD,
    LD_WRITEREG,
    LD_READREG
};

namespace prog{
    void sys(unsigned short addr);
    void cls();
    void ret();
    void jmp(unsigned short addr);
    void call(unsigned short addr);
    void seByte(int reg, unsigned char byte);
    void sneByte(int reg, unsigned char byte);

    int parse(char& instruciton);
    void run();
}