#include "chip8.h"

int main(int argc, char** argv){
  Emulator chip8;

  if(argc > 1){
    chip8.loadFile(*(argv+1));
  }else{
    chip8.loadBytes({}, 0);
  }
  
  chip8.execute();
  chip8.displayState();
  return 0;
}