#include <chip8.hpp>
#include <vector>
#include <fstream>
#include <random>
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
    uint16_t inst = (memory[pc] << 8) | memory[pc+1];
    pc += 2;
    switch(inst & 0xF000){
        case 0x0000:
            switch(inst & 0x00FF){
                case 0x00E0: // clear the screen
                    display.fill(0); 
                    break; 
                case 0x00EE: // returning from function
                    pc = stack.top();
                    stack.pop();
                    break; 
            }
            break; 
        case 0x1000: // PC jump
            pc = (inst & 0x0FFF); 
            break;
        case 0x2000: // function call
            stack.push(pc);
            pc = (inst & 0x0FFF); 
            break;
        case 0x3000:
            if(V[(inst & 0x0F00) >> 8] == (inst & 0x00FF))
                pc += 2;
            break;
        case 0x4000:
            if(V[(inst & 0x0F00) >> 8] != (inst & 0x00FF))
                pc += 2;
            break;
        case 0x5000:
            if(V[(inst & 0x0F00) >> 8] == V[(inst & 0x00F0) >> 4])
                pc += 2;
            break;
        case 0x6000: //SET V[X] = 00NN
            V[(inst & 0x0F00) >> 8] = (inst & 0x00FF); 
            break;
        case 0x7000: //ADD NN to V[X]
            V[(inst & 0x0F00) >> 8] += (inst & 0x00FF); 
            break;
        case 0x8000:
            switch(inst & 0x000F){
                case 0x0:
                    V[(inst & 0x0F00) >> 8] = V[(inst & 0x00F0) >> 4];
                    break;
                case 0x1:
                    V[(inst & 0x0F00) >> 8] |= V[(inst & 0x00F0) >> 4];
                    break;
                case 0x2:
                    V[(inst & 0x0F00) >> 8] &= V[(inst & 0x00F0) >> 4];
                    break;
                case 0x3:
                    V[(inst & 0x0F00) >> 8] ^= V[(inst & 0x00F0) >> 4];
                    break;
                case 0x4:
                    V[0xF] = (V[(inst & 0x00F0) >> 4] > (0xFF - V[(inst & 0x0F00) >> 8])) ? 1 : 0;
                    V[(inst & 0x0F00) >> 8] += V[(inst & 0x00F0) >> 4];
                    break;
                case 0x5:
                    V[0xF] = (V[(inst & 0x00F0) >> 4] > V[(inst & 0x0F00) >> 8]) ? 1 : 0;
                    V[(inst & 0x0F00) >> 8] -= V[(inst & 0x00F0) >> 4];
                    break;
                case 0x6:
                    V[(inst & 0x0F00) >> 8] = V[(inst & 0x00F0) >> 4];
                    if(V[(inst & 0x0F00) >> 8] & 0x1)
                        V[0xF] = 1;
                    V[(inst & 0x0F00) >> 8] >>= 1;
                    break;
                case 0x7:
                    V[(inst & 0x0F00) >> 8] = V[(inst & 0x00F0) >> 4] - V[(inst & 0x0F00) >> 8];
                    break;
                case 0xE:
                    V[(inst & 0x0F00) >> 8] = V[(inst & 0x00F0) >> 4];
                    if(V[(inst & 0x0F00) >> 8] & 0x8)
                        V[0xF] = 1;
                    V[(inst & 0x0F00) >> 8] <<= 1;
                    break;
            }
            break;
        case 0x9000:
            if(V[(inst & 0x0F00) >> 8] != V[(inst & 0x00F0) >> 4])
                pc += 2;
            break;
        case 0xA000: // SET I = 0NNN
            I = inst & 0x0FFF;
            break;
        case 0xB000:
            pc = (inst & 0x0FFF) + V[0x0];
            break;
        case 0xC000:{
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, 0xFF);
            uint8_t rand_num = distrib(gen);
            V[(inst & 0x0F00) >> 8] = rand_num & (inst & 0x00FF);
            break;
        }
        case 0xD000:{
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
        }
            break;
        case 0xE000:
            switch(inst & 0x00FF){
                case 0x9E:
                    if(keys[V[(inst & 0x0F00) >> 8]])
                        pc += 2;
                    break;
                case 0XA1:
                    if(!keys[V[(inst & 0x0F00) >> 8]])
                        pc += 2;
                    break;
            }
            break;
        case 0xF000:
            switch(inst & 0x00FF){
                case 0x07:
                    V[(inst & 0x0F00) >> 8] = delay_timer;                    
                    break;
                case 0x0A:{
                    bool keypressed = false;
                    for(uint8_t i = 0; i <= 0xF; i++)
                        if(keys[i]){
                            keypressed = true;
                            V[(inst & 0x0F00) >> 8] = i; 
                        }
                    if(!keypressed)
                        pc -= 2;
                    break;
                }
                case 0x15:
                    delay_timer = V[(inst & 0x0F00) >> 8];
                    break;
                case 0x18:
                    sound_timer = V[(inst & 0x0F00) >> 8];
                    break;
                case 0x1E:{
                    I += V[(inst & 0x0F00) >> 8];
                    if(I & 0xF000) V[0xF] = 1;
                    break;
                }
                case 0x29:
                    I = V[(inst & 0x0F00) >> 8]*5 + 0x50;
                    break;
                case 0x33:{
                    uint8_t vx = V[(inst & 0x0F00) >> 8];
                    memory[I+2] = vx % 10; vx /= 10;
                    memory[I+1] = vx % 10; vx /= 10;
                    memory[I] = vx;
                    break;
                }
                case 0x55:
                    for(uint8_t i = 0; i <= ((inst & 0x0F00) >> 8); i++){
                        memory[i + I] = V[i];
                    }
                    I += ((inst & 0x0F00) >> 8) + 1;
                    break;
                case 0x65:
                    for(uint8_t i = 0; i <= ((inst & 0x0F00) >> 8); i++){
                        V[i] = memory[i + I] ;
                    }
                    I += ((inst & 0x0F00) >> 8) + 1;
                    break;
            }
            break;
        default:
            throw std::runtime_error("Invalid instruction");
    }
    if(delay_timer > 0)
        delay_timer--;
    if(sound_timer > 0){
        if(sound_timer == 1){

        }
            
        sound_timer--;
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
    if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased){
        bool key_pressed = (event.type == sf::Event::KeyPressed);
        switch(event.key.code){
            case sf::Keyboard::Num1:
                keys[0x1] = key_pressed;
                break;
            case sf::Keyboard::Num2:
                keys[0x2] = key_pressed;
                break;
            case sf::Keyboard::Num3:
                keys[0x3] = key_pressed;
                break;
            case sf::Keyboard::Num4:
                keys[0xC] = key_pressed;
                break;
            case sf::Keyboard::Q:
                keys[0x4] = key_pressed;
                break;
            case sf::Keyboard::W:
                keys[0x5] = key_pressed;
                break;
            case sf::Keyboard::E:
                keys[0x6] = key_pressed;
                break;
            case sf::Keyboard::R:
                keys[0xD] = key_pressed;
                break;
            case sf::Keyboard::A:
                keys[0x7] = key_pressed;
                break;
            case sf::Keyboard::S:
                keys[0x8] = key_pressed;
                break;
            case sf::Keyboard::D:
                keys[0x9] = key_pressed;
                break;
            case sf::Keyboard::F:
                keys[0xE] = key_pressed;
                break;
            case sf::Keyboard::Z:
                keys[0xA] = key_pressed;
                break;
            case sf::Keyboard::X:
                keys[0x0] = key_pressed;
                break;
            case sf::Keyboard::C:
                keys[0xB] = key_pressed;
                break;
            case sf::Keyboard::V:
                keys[0xF] = key_pressed;
                break;
            default:
                break;
        }
    }
}