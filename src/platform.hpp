#pragma once

#include <SDL.h>
#include <glad/gl.h>
#include <cstdint>

class Platform {
    public:
        Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
        ~Platform();

    private:
        SDL_Window* window{};
        SDL_Renderer* renderer{};
        SDL_Texture* texture{};
}