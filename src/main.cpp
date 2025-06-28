#define SDL_MAIN_HANDLED
#undef main
#include "chip.hpp"
#include "platform.hpp"
#include <chrono>
#include <iostream>
#include <algorithm>  

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const* romFilename = argv[3];

    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

    Chip8 chip8;
    memset(chip8.video, 0, sizeof(chip8.video));
    chip8.drawFlag = true;
    chip8.LoadROM(romFilename);
    static uint32_t pixels[VIDEO_WIDTH * VIDEO_HEIGHT];
    int videoPitch = sizeof(uint32_t) * VIDEO_WIDTH;
    
    std::cout << "Emulation starting..." << std::endl;
    
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit) {
        quit = platform.ProcessInput(chip8.keypad);
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
    
        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            chip8.Cycle();

            if (chip8.drawFlag) {
                std::cout << "Draw flag is set, updating display\n";
                // Check first few pixels in video buffer
                std::cout << "First 10 pixels: ";
                for (int i = 0; i < 10; i++) {
                    std::cout << (chip8.video[i] ? "■" : "□");
                }
                std::cout << std::endl;
            }
            
            if (chip8.drawFlag) {
                for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
                    // 0xFF000000 = Black (Alpha=255, R=0, G=0, B=0)
                    // 0xFFFFFFFF = White (Alpha=255, R=255, G=255, B=255)
                    pixels[i] = chip8.video[i] ? 0xFFFFFFFF : 0xFF000000;
                }
                
                platform.Update(pixels, videoPitch);
                chip8.drawFlag = false;  
            }
        }
    }

    return 0;   
}