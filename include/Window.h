#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);


    ~Window();

    // Will swap the buffers
    void swapBuffers() const;

    // Processes the events and returns if the events are processed
    bool processEvents();

    // Returns the width of the window
    int getWidth() const;
    // Returns the width of the window
    int getHeight() const;

private:
    SDL_Window*    m_window      = nullptr;
    SDL_GLContext  m_context     = nullptr;
    const int      m_width       = 0;
    const int      m_height      = 0;
    bool           m_shouldClose = false;

    // This wil initiate SDL’s video subsystem
    void initSDL();
};
