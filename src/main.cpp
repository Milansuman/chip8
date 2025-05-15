#include "chip8.h"

int main(){
  Emulator chip8;

  chip8.loadFile("./2-ibm-logo.ch8");
  chip8.execute();
  chip8.displayState();
  return 0;
}