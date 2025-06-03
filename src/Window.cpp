#include "../include/src/Window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

#include "imgui_impl_sdl2.h"
#include "../include/src/Shader.h"

Window::Window(const int width, const int height, const std::string& title) : m_width(width), m_height(height) {

    // Initializing SDL’s video subsystem
    initSDL();

    // Create the SDL window
    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!m_window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        m_shouldClose = true;
        return;
    }

    // Create OpenGL context
    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context) {
        std::cerr << "Failed to create GL context: " << SDL_GetError() << std::endl;
        m_shouldClose = true;
        return;
    }

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        m_shouldClose = true;
        return;
    }

    // Enables VSync (Set SDL_GL_SetSwapInterval(0) to disable VSync)
    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cerr << "Failed to set swap interval: " << SDL_GetError() << std::endl;
    }
}


Window::~Window() {
    if (m_context) {
        SDL_GL_DeleteContext(m_context);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
    SDL_Quit();
}

void Window::swapBuffers() const {
    if (m_window) {
        SDL_GL_SwapWindow(m_window);
    }
}


void Window::setShouldClose(bool shouldClose)
{
    m_shouldClose = shouldClose;
}

int Window::getWidth() const
{
    return m_width;
}

int Window::getHeight() const
{
    return m_height;
}

SDL_GLContext Window::getGLContext() const
{
    return m_context;
}

SDL_Window *Window::getSDLWindow() const
{
    return m_window;
}

void Window::initSDL() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        m_shouldClose = true;
        return;
    }


    // Request to use opengl version 4.6
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // Request a core (No deprecated functions)
    SDL_GL_SetAttribute(
    SDL_GL_CONTEXT_PROFILE_MASK,
    SDL_GL_CONTEXT_PROFILE_CORE
    );

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


    // SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
    SDL_SetWindowKeyboardGrab(m_window, SDL_TRUE);
    // SDL_ShowCursor(SDL_DISABLE);
    // SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowGrab(m_window, SDL_TRUE);
}