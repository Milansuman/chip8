#include "chip8.h"
#include "src/cpu.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>

Emulator::Emulator(){
  this->vmem = std::shared_ptr<std::vector<unsigned char>>(new std::vector<unsigned char>(256, 0));

  this->cpu = CPU(this->vmem);

  this->window = sf::RenderWindow(sf::VideoMode({64*15, 32*15}), "MCHIP 8");
  this->window.setFramerateLimit(1000);
}

void Emulator::execute(){
  while (this->window.isOpen()) {
    while (const std::optional event = this->window.pollEvent()) {
      // Request for closing the window
      if (event->is<sf::Event::Closed>()){
        this->window.close();
      }else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
        std::cout << "pressed\n";
        if(this->cpu.keyInt){
          this->cpu.pressedKey = keyPressed->code;
        }
      }
    }
    this->cpu.tick();
    this->window.clear(sf::Color(84, 9, 218,255));
    this->draw();
  }
}

void Emulator::draw(){
  for(int y=0; y<32; y++){
    for(int x=0; x<8; x++){
      unsigned char byte = this->vmem->at(8*y+x);

      if(!byte){
        continue;
      }

      for(int i=0; i<8; i++){
        if(byte & 0b10000000){
          sf::RectangleShape pixel({15.0f, 15.0f});
          pixel.setFillColor(sf::Color(187, 251, 255, 255));
          pixel.setPosition({(float) (x*8+i)*15, (float) y*15});
          this->window.draw(pixel);
        }
        byte <<= 1;
      }
    }
  }
  this->window.display();
}

void Emulator::loadBytes(const unsigned char bytes[], size_t n){
  this->cpu.loadBytes(bytes, n);
}

void Emulator::loadFile(const char* filename){
  this->cpu.loadFile(filename);
}

void Emulator::displayState(){
  this->cpu.displayState();

  std::cout << "\nVideo Memory (vmem) contents:" << std::endl;
  for(size_t i = 0; i < this->vmem->size(); ++i){
    if(i % 8 == 0 && i != 0){
      std::cout << std::endl;
    }
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(this->vmem->at(i)) << " ";
  }
  std::cout << std::dec << std::endl; // Reset to decimal for future cout operations
}

