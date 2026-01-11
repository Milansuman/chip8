#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <memory>

#include "cpu.h"

class Emulator{
private:
  std::shared_ptr<std::vector<unsigned char>> vmem;
  std::unique_ptr<sf::RenderWindow> window;
  CPU cpu;

  void draw();

public:
  Emulator();
  void execute();
  void loadBytes(const unsigned char bytes[], size_t n);
  void loadFile(const char* filename);
  void displayState();
};