#include "../include/src/EventDispatcher.h"

#include "imgui_impl_sdl2.h"


bool EventDispatcher::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Check if the window should close
        if (event.type == SDL_QUIT) {
            m_closeEvent = true;
        }
        const Uint8* kb = SDL_GetKeyboardState(nullptr);
        if (kb[SDL_SCANCODE_ESCAPE]) {
            m_closeEvent = true;
            return false;
        }

        // Update the imGUI event
        ImGui_ImplSDL2_ProcessEvent(&event);

        m_camera.handleEvent(event);
        m_camera.update(0.01);
    }

    if (m_closeEvent) {
        m_window.setShouldClose(m_closeEvent);
    }

    return !m_closeEvent;

}
