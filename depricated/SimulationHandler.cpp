#include "SimulationHandler.h"

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

    glGenBuffers(1, &m_ssboEnergy);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboEnergy);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec2) * m_numBodies, nullptr, GL_DYNAMIC_READ); // x = KE, y = PE
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_numWorkGroups = (m_numBodies + m_workGroupSize - 1) / m_workGroupSize;

    m_computeShader.use();
    m_computeShader.setUniform("dt", 0.01f);
    m_computeShader.setUniform("G", 1 * m_G); // Gravitational constant
    m_computeShader.setUniform("epsilon", m_softening);

    Shader::unUse();
}

SimulationHandler::~SimulationHandler() {
    if (m_ssboBodies != 0) {
        glDeleteBuffers(1, &m_ssboBodies);
        m_ssboBodies = 0;
    }
}


void SimulationHandler::newSimulation(const int bodyCount) {
    m_numBodies = bodyCount;
    hostBodies.clear();
    hostBodies.reserve(bodyCount);

    for (int i = 0; i < bodyCount; ++i) {
        auto [pos, vel] = randomPointAndVelocityInGalaxy();
        hostBodies.emplace_back(Body(glm::vec4(pos, 0), glm::vec4(vel, 1)));
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboBodies);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        sizeof(Body) * m_numBodies,
        hostBodies.data(),
        GL_DYNAMIC_DRAW
    );
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboEnergy);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec2) * m_numBodies, nullptr, GL_DYNAMIC_READ); // x = KE, y = PE
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_numWorkGroups = (m_numBodies + m_workGroupSize - 1) / m_workGroupSize;
    m_energy.clear();
}



void SimulationHandler::step() {
    m_computeShader.use();

    // 3) Bind our SSBO at binding point 0 (must match 'binding = 0' in GLSL)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboBodies);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssboEnergy);
    // 4) Dispatch enough work‐groups to cover all bodies
    glDispatchCompute(m_numWorkGroups, 1, 1);

    // 5) Insert a barrier so that subsequent rendering sees updated positions
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    auto [totalKE, totalPE] = computeEnergiesFromGPU();

    m_energy.push_back(totalKE + totalPE);

    Shader::unUse();
}


std::pair<glm::vec3, glm::vec3> SimulationHandler::randomPointAndVelocityInGalaxy() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    static std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * glm::pi<float>());

    float theta = angleDist(gen); // azimuthal angle [0, 2pi)
    float phi = acos(1.0f - 2.0f * dist(gen)); // inclination [0, pi], uniform on sphere

    float x = 10 * sin(phi) * cos(theta);
    float y = 10 * sin(phi) * sin(theta);
    float z = 10 * cos(phi);

    glm::vec3 position = glm::vec3(x, y, z);
    return {position, glm::vec3(0, 0, 0)};
}

std::pair<float, float> SimulationHandler::computeEnergiesFromGPU() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboEnergy);
    auto* data = static_cast<glm::vec2 *>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

    float totalKE = 0.0f;
    float totalPE = 0.0f;

    for (int i = 0; i < m_numBodies; ++i) {
        totalKE += data[i].x;
        totalPE += data[i].y;
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return { totalKE, 0.5f * totalPE }; // correct PE double-counting
}