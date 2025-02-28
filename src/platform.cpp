#include "platform.hpp"
#include <SDL.h>
#include <glad/gl.h>

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowWidth, windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    
}

Platform::~Platform() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}