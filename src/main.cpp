// main.cpp
#include <iostream>
#include <vector>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "SimulationHandler.h"



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
    while (window.processEvents()) {

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
}