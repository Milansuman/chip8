#include "cpu.h"

int main(){
  CPU cpu;

  cpu.loadBytes((unsigned char[]){0x60, 0xFF, 0x61, 0xFF, 0x80, 0x14}, 6);
  cpu.displayState();
  cpu.execute();
  cpu.displayState();
  return 0;
}