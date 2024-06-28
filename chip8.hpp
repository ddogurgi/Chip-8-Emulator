#include <iostream>
#include <array>
#include <stack>
#include <SFML/Graphics.hpp>


class Chip8 {
    public:
        void Initialize();
        void LoadProgram(const std::string& filename);
        void EmulateCycle();
        void DrawGraphics(sf::RenderWindow &window);
        void SetKeys(sf::Event &event);

    private:
        std::array<uint8_t, 4096> memory{};//4KB direct-access Memory 
        //std::array<bool, 64*32> display{}; //64x32 monochrome pixels of display
        
        uint16_t I = 0; //16-bit address register
        uint16_t pc = 0x200; //16-bit program counter(starts at 0x200)
        std::stack<uint16_t> stack; // A stack which is used to call subroutines/functions and return for them
        uint8_t delay_timer; //8-bit delay timer, dec at a rate of 60Hz
        uint8_t sound_timer; //8-bit sound timer
        
        std::array<uint8_t, 16> V{}; //16 8-bit general-purpose registers
        
};
