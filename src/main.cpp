// main.cpp
#include <iostream>
#include <vector>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "Window.h"
#include "../include/src/Shader.h"
#include "../include/src/Camera.h"
#include "../include/src/SimulationHandler.h"



int main(int argc, char* argv[]) {

    Window window(800, 600, "Nbody");
    Camera camera(window);


    SimulationHandler simulation(1000); // Initialize with 1000 bodies


    Shader renderer = Shader({
        {"../src/shaders/bil.geom",GL_GEOMETRY_SHADER},
        {"../src/shaders/bil.vert", GL_VERTEX_SHADER},
        {"../src/shaders/bil.frag", GL_FRAGMENT_SHADER}
    });


    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    Uint32 lastTime = SDL_GetTicks();
    int frames = 0;

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), window.getGLContext());
    ImGui_ImplOpenGL3_Init("#version 460");

    while (window.processEvents()) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Show the ImGui demo window
        ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        Uint32 currentTime = SDL_GetTicks();
        frames++;

        if (currentTime - lastTime >= 100) { // 1 second has passed
            std::cout << "FPS: " << frames * 10 << std::endl;
            frames = 0;
            lastTime = currentTime;
        }

        camera.update(0.1);

        simulation.step();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. Use the render shader
        renderer.use();

        glm::mat4 viewMat = camera.getViewMatrix();
        glm::mat4 projMat = camera.getProjectionMatrix();
        glm::mat3 basis = camera.getBasisVectors();
        renderer.setUniform("u_view", viewMat);
        renderer.setUniform("u_proj", projMat);
        renderer.setUniform("u_camBasis", basis);

        glBindVertexArray(vao);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, simulation.getBoddySSBO());
        glDrawArrays(GL_POINTS, 0, 1000);
        glBindVertexArray(0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        Shader::unUse();
        window.swapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(window.getGLContext());
    SDL_DestroyWindow(window.getSDLWindow());
    SDL_Quit();

}