#include <SFML/Window.hpp>
#include "hardware.hpp"

int main(){
    chip8::hardware hardware;
    sf::Window window;
    window.create(sf::VideoMode(1080, 720), "Milan's CHIP8 Emulator");
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }
    }
    
    return 0;
}