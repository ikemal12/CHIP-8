#pragma once

#include <SDL.h>
#include <glad/gl.h>
#include <cstdint>

class Platform {
    public:
        Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
        ~Platform();
        void Update(void const* buffer, int pitch);

    private:
        SDL_Window* window{};
        SDL_Renderer* renderer{};
        SDL_Texture* texture{};
        SDL_GLContext gl_context{};
        GLuint framebuffer_texture;
}