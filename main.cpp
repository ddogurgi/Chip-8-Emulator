#include <chip8.hpp>

int main(int argc, char** argv){
    Chip8 chip8;

    chip8.Initialize();
    std::string file_name = (argc > 1) ? argv[1] : "ch8s/IBM_Logo.ch8";
    chip8.LoadProgram(file_name);
    sf::RenderWindow window(sf::VideoMode(640, 320), "CHIP-8 Emulator");

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
            chip8.SetKeys(event);
        }
        chip8.EmulateCycle();
        if (chip8.drawflag) {
            window.clear();
            chip8.DrawGraphics(window);
            window.display();
            chip8.drawflag = false;
        }
        sf::sleep(sf::microseconds(500));
       
    }
}