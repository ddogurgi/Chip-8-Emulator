#include <chip8.hpp>
#include <vector>
#include <fstream>

void Chip8::Initialize(){
    uint8_t const font[0x50] {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for(int i = 0x50; i <= 0x9F; i++)
        this->memory[i] = font[i-0x50]; //font 050 to 09F by convention (it is fine to put it at 000-1FF)
}

void Chip8::LoadProgram(const std::string& filename){
    //Read the program
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(!file.is_open())
        throw std::runtime_error("Could not open the file");

    //Load the program on the memory

    std::streamsize size = file.tellg(); //When we read the file, by ios::ate, the current position is the end of the file. So, tellg() returns size
    file.seekg(0, std::ios::beg); //Set the current position to zero(start of the file)

    std::vector<char> buffer(size);
    if(file.read(buffer.data(),size)){
        for(int i = 0; i < size; i++)
            memory[i+0x200] = buffer[i]; //Load 0x200~ 
    }
}

void Chip8::EmulateCycle(){
    //fetch
    uint16_t inst = (this->memory[this->pc] << 8) | memory[this->pc+1];
    this->pc += 2;
    switch(inst & 0xF000){
        case 0x0000:
            switch(inst & 0x00FF){
                case 0x00E0: // clear the screen
                    display.fill(0); 
                    break; 
                case 0x00EE: // returning from function
                    this->pc = this->stack.top();
                    this->stack.pop();
                    break; 
            }
            break; 
        case 0x1000: // PC jump
            this->pc = (inst & 0x0FFF); 
            break;
        case 0x2000: // function call
            this->stack.push(this->pc);
            this->pc = (inst & 0x0FFF); 
            break;
        case 0x3000:
            break;
        case 0x4000:
            break;
        case 0x5000:
            break;
        case 0x6000: //SET V[X] = 00NN
            V[(inst & 0x0F00) >> 8] = (inst & 0x00FF); 
            break;
        case 0x7000: //ADD NN to V[X]
            V[(inst & 0x0F00) >> 8] += (inst & 0x00FF); 
            break;
        case 0x8000:
            break;
        case 0x9000:
            break;
        case 0xA000: // SET I = 0NNN
            I = inst & 0x0FFF;
            break;
        case 0xB000:
            break;
        case 0xC000:
            break;
        case 0xD000:
            uint8_t X = V[(inst & 0x0F00) >> 8] & 0x003F;
            uint8_t Y = V[(inst & 0x00F0) >> 4] & 0x001F;
            V[0x000F] = 0;
            uint8_t N = inst & 0x000F;
            uint8_t pixel;
            for(int y = Y; y < std::min(Y+N, 32); y++){
                pixel = memory[I + (y-Y)];
                for(int x = X; x < std::min(X+8, 64); x++){
                    if((pixel & (0x80 >> (x-X))) != 0){
                        if(display[x + y*64]){
                            V[0x000F] = 1;
                        }
                        display[x + y*64] ^= true;
                    }
                }
            }
            break;
        /*
        case 0xE000:
            break;
        case 0xF000:
            break;
        */
    }    
}

void Chip8::DrawGraphics(sf::RenderWindow &window){
    window.clear(sf::Color::Black);

    sf::RectangleShape pixel(sf::Vector2f(10, 10));
    pixel.setFillColor(sf::Color::White);

    for(int y = 0; y < 32; y++)
        for(int x = 0; x < 64; x++)
            if(display[x+y*64]){
                pixel.setPosition(x*10, y*10);
                window.draw(pixel);
            }
    
    window.display();
}

void Chip8::SetKeys(sf::Event &event){
    
}