// main.cpp
#include <iostream>
#include <vector>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "EventDispatcher.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "NewtonianSimulationHandler.h"
#include "Window.h"
#include "../include/src/Shader.h"
#include "../include/src/Camera.h"
#include "../depricated/SimulationHandler.h"



int main(int argc, char* argv[]) {

    Window window(1920, 1080, "Nbody");
    Camera camera(window);
    static int bodyCount = 1000;
    static IntegrationType currentType= IntegrationType::EULER;

    NewtonianSimulationHandler simulation = NewtonianSimulationHandler(bodyCount, currentType); // Initialize with 1000 bodies

    EventDispatcher dispatcher = EventDispatcher(window, camera);

    Shader renderer = Shader({
        {"../src/shaders/bil.geom",GL_GEOMETRY_SHADER},
        {"../src/shaders/bil.vert", GL_VERTEX_SHADER},
        {"../src/shaders/bil.frag", GL_FRAGMENT_SHADER}
    });


    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    Uint32 lastTime = SDL_GetTicks();


    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window.getSDLWindow(), window.getGLContext());
    ImGui_ImplOpenGL3_Init("#version 460");

    static bool running = false;
    while (dispatcher.processEvents()) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        Uint32 currentTime = SDL_GetTicks();
        //std::cout << currentTime - lastTime << " ms" << std::endl;
        lastTime = currentTime;


        if (running) {simulation.tryStepSimulation();}

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
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, simulation.getBodySSBO());
        glDrawArrays(GL_POINTS, 0, simulation.getNumBodies());
        glBindVertexArray(0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        Shader::unUse();



        if (ImGui::SliderInt("Number of Bodies", &bodyCount, 10, 100000)) {
            simulation.newSimulation(bodyCount, LEAPFROG);
            running = false;
        }

        const char* integrationTypes[] = {"Euler", "Leapfrog"};
        int modeIndex = static_cast<int>(currentType);
        if (ImGui::Combo("Select integration type", &modeIndex, integrationTypes, IM_ARRAYSIZE(integrationTypes))) {
            currentType = static_cast<IntegrationType>(modeIndex);
            simulation.newSimulation(bodyCount, currentType);
            running = false;
        }

        ImGui::Checkbox("Run Simulation", &running);
        std::vector<float> m_energy = simulation.getTotalEnergy();



        if (!m_energy.empty()) {
            ImGui::PlotLines("Energy Over Time (Stable Part)",
                             m_energy.data(),
                             m_energy.size(),
                             0, nullptr,
                             FLT_MAX, FLT_MAX,
                             ImVec2(0, 100));
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(window.getGLContext());
    SDL_DestroyWindow(window.getSDLWindow());
    SDL_Quit();

}