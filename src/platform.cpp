#include "platform.hpp"
#include <SDL.h>
#include <cstring>
#include <iostream>

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,  
        SDL_TEXTUREACCESS_STREAMING,
        textureWidth, textureHeight
    );
    
    if (!texture) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    std::cout << "SDL initialization successful" << std::endl;
}

Platform::~Platform() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) {
    if (!buffer || !texture || !renderer) {
        std::cerr << "Invalid state in Update" << std::endl;
        return;
    }
    
    if (SDL_UpdateTexture(texture, nullptr, buffer, pitch) != 0) {
        std::cerr << "Failed to update texture: " << SDL_GetError() << std::endl;
        return;
    }
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys) {
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                quit = true;
            } break;

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: {
                        quit = true;
                    } break;

                    case SDLK_x: {
                        keys[0] = 1;
                    } break;

                    case SDLK_1: { keys[1] = 1; } break;
                    case SDLK_2: { keys[2] = 1; } break;
                    case SDLK_3: { keys[3] = 1; } break;
                    case SDLK_q: { keys[4] = 1; } break;
                    case SDLK_w: { keys[5] = 1; } break;
                    case SDLK_e: { keys[6] = 1; } break;
                    case SDLK_a: { keys[7] = 1; } break;
                    case SDLK_s: { keys[8] = 1; } break;
                    case SDLK_d: { keys[9] = 1; } break;
                    case SDLK_z: { keys[0xA] = 1; } break;
                    case SDLK_c: { keys[0xB] = 1; } break;
                    case SDLK_4: { keys[0xC] = 1; } break;
                    case SDLK_r: { keys[0xD] = 1; } break;
                    case SDLK_f: { keys[0xE] = 1; } break;
                    case SDLK_v: { keys[0xF] = 1; } break;
                }
            } break;

            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                    case SDLK_x: { keys[0] = 0; } break;
                    case SDLK_1: { keys[1] = 0; } break;
                    case SDLK_2: { keys[2] = 0; } break;
                    case SDLK_3: { keys[3] = 0; } break;
                    case SDLK_q: { keys[4] = 0; } break;
                    case SDLK_w: { keys[5] = 0; } break;
                    case SDLK_e: { keys[6] = 0; } break;
                    case SDLK_a: { keys[7] = 0; } break;
                    case SDLK_s: { keys[8] = 0; } break;
                    case SDLK_d: { keys[9] = 0; } break;
                    case SDLK_z: { keys[0xA] = 0; } break;
                    case SDLK_c: { keys[0xB] = 0; } break;
                    case SDLK_4: { keys[0xC] = 0; } break;
                    case SDLK_r: { keys[0xD] = 0; } break;
                    case SDLK_f: { keys[0xE] = 0; } break;
                    case SDLK_v: { keys[0xF] = 0; } break;
                }
            } break;
        }
    }

    return quit;
}