//
// Created by dorito009 on 6/3/25.
//

#include "../include/SimulationHandler.h"

#include <iostream>
#include <random>
#include <bits/ostream.tcc>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>


SimulationHandler::SimulationHandler(const int numBodies) : m_numBodies(numBodies), m_computeShader(Shader({{"../src/shaders/NBody.comp",GL_COMPUTE_SHADER}})) {
    hostBodies.reserve(numBodies);
    for (int i = 0; i < numBodies; ++i) {

        auto [pos, vel] = randomPointAndVelocityInGalaxy();

        hostBodies.emplace_back(Body(glm::vec4(pos, 0), glm::vec4(vel, 1)));
    }


    glGenBuffers(1, &m_ssboBodies);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboBodies);

    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        sizeof(Body) * m_numBodies,
        hostBodies.data(),
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    m_numWorkGroups = (m_numBodies + m_workGroupSize - 1) / m_workGroupSize;

    m_computeShader.use();
    m_computeShader.setUniform("dt", 0.0001 * m_dt);
    m_computeShader.setUniform("G", 1 * m_G); // Gravitational constant
    m_computeShader.setUniform("epsilon", m_softening);

    Shader::unUse();
}

void SimulationHandler::step() const {
    m_computeShader.use();

    // 3) Bind our SSBO at binding point 0 (must match 'binding = 0' in GLSL)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboBodies);
    // 4) Dispatch enough work‐groups to cover all bodies
    glDispatchCompute(m_numWorkGroups, 1, 1);

    // 5) Insert a barrier so that subsequent rendering sees updated positions
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    Shader::unUse();
}


std::pair<glm::vec3, glm::vec3> SimulationHandler::randomPointAndVelocityInGalaxy() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    static std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * glm::pi<float>());
    static std::normal_distribution<float> heightDist(0.0f, 2.0f); // thinner vertical height

    // Disk radius: exponential falloff
    float r = 30.0f * sqrt(dist(gen));  // sqrt to bias towards center
    float theta = angleDist(gen);       // azimuthal angle

    // Cartesian coordinates in the disk
    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = heightDist(gen);          // small vertical spread

    glm::vec3 position = glm::vec3(x, y, z);

    // Create a tangential velocity (approximate circular orbit)
    glm::vec3 radial = glm::normalize(glm::vec3(x, y, 0.0f));
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 tangential = glm::normalize(glm::cross(up, radial)); // tangential in XY

    float velocityMagnitude = glm::sqrt(1000.0f / (r + 1.0f)); // simple orbital falloff
    glm::vec3 velocity = tangential * velocityMagnitude;

    return { position, velocity };
}