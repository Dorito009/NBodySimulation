//
// Created by dorito009 on 6/3/25.
//

#include "../include/src/NewtonianSimulationHandler.h"

#include <iostream>
#include <random>
#include <bits/ostream.tcc>

NewtonianSimulationHandler::NewtonianSimulationHandler(const int numBodies, IntegrationType integrationType)
    :m_numBodies(numBodies)
    ,m_integrationType(integrationType)
    ,m_eulerComputeShader(Shader({{EULER_PATH, GL_COMPUTE_SHADER}}))
    ,m_leapfrogComputeShader1(Shader({{LEAPFROG1_PATH, GL_COMPUTE_SHADER}}))
    ,m_leapfrogComputeShader2(Shader({{LEAPFROG2_PATH, GL_COMPUTE_SHADER}}))
{
    initiateBodies();
    setupShaders();
}

NewtonianSimulationHandler::~NewtonianSimulationHandler() {
    if (m_ssboBodies != 0) {
        glDeleteBuffers(1, &m_ssboBodies);
        m_ssboBodies = 0;
    }
    if (m_ssboEnergies != 0) {
        glDeleteBuffers(1, &m_ssboEnergies);
        m_ssboEnergies = 0;
    }
}


void NewtonianSimulationHandler::newSimulation(const int numBodies, IntegrationType integrationType) {
    m_running = true;

    m_numBodies = numBodies;
    m_integrationType = integrationType;

    initiateBodies();
    setupShaders();
}

bool NewtonianSimulationHandler::tryStepSimulation() {
    //if (!m_running) {return m_running;}
    if (m_integrationType == EULER) {
        m_eulerComputeShader.use();

        // Bind the SSBOs for bodies and energies
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboBodies);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssboEnergies);

        // Dispatch the work‐groups
        glDispatchCompute(m_numWorkGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    else if (m_integrationType == LEAPFROG) {
        m_leapfrogComputeShader1.use();
        // Bind the SSBOs for bodies and energies
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboBodies);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssboEnergies);

        // Dispatch the work‐groups
        glDispatchCompute(m_numWorkGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_leapfrogComputeShader2.use();
        // Bind the SSBOs for bodies and energies
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboBodies);


        // Dispatch the work‐groups
        glDispatchCompute(m_numWorkGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    computeEnergy();
    Shader::unUse();
    return true;
}

void NewtonianSimulationHandler::computeEnergy() {
    // Bind and read the SSBO for energies
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboEnergies);
    auto* data = static_cast<glm::vec2 *>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

    float totalKE = 0.0f;
    float totalPE = 0.0f;

    for (int i = 0; i < m_numBodies; ++i) {
        totalKE += data[i].x;
        totalPE += data[i].y;
    }
    totalPE *= 0.5f;

    // Unbind the SSBO and unmap the buffer
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_energies.emplace_back(totalKE, totalPE);
}


void NewtonianSimulationHandler::initiateBodies() {
    m_bodies.clear();
    m_energies.clear();
    m_bodies.reserve(m_numBodies);

    static std::random_device rd;
    static std::mt19937       gen(rd());
    static std::uniform_real_distribution<float> rand(0.0f, 1.0f);

    for (int i = 0; i < m_numBodies; ++i) {

        float r = 20 * std::sqrt(rand(gen)) + 8.0f;
        float theta = rand(gen) * 2.0f * M_PI;

        float x = r * std::cos(theta);
        float z = r * std::sin(theta);

        glm::vec4 position(x, 0, z, 0.0f);
        glm::vec4 impulse(0, 0, 0, 1.0f);

        m_bodies.emplace_back(position, impulse);
    }
}

void NewtonianSimulationHandler::setIntegrationType() const {
    if (m_integrationType == EULER) {
        m_eulerComputeShader.use();
        m_eulerComputeShader.setUniform("dt", m_dt);
        m_eulerComputeShader.setUniform("G", m_g);
        m_eulerComputeShader.setUniform("epsilon", m_softening);
    } else if (m_integrationType == LEAPFROG) {
        m_leapfrogComputeShader1.use();
        m_leapfrogComputeShader1.setUniform("dt", m_dt);
        m_leapfrogComputeShader1.setUniform("G", m_g);
        m_leapfrogComputeShader1.setUniform("epsilon", m_softening);

        m_leapfrogComputeShader2.use();
        m_leapfrogComputeShader2.setUniform("dt", m_dt);
        m_leapfrogComputeShader2.setUniform("G", m_g);
        m_leapfrogComputeShader2.setUniform("epsilon", m_softening);
    }
    Shader::unUse();
}

void NewtonianSimulationHandler::setupShaders() {
    // Setup the ssbo for the bodies
    glGenBuffers(1, &m_ssboBodies);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboBodies);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        sizeof(Body) * m_numBodies,
        m_bodies.data(),
        GL_DYNAMIC_DRAW
    );
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &m_ssboEnergies);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboEnergies);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec2) * m_numBodies, nullptr, GL_DYNAMIC_READ); // x = KE, y = 2*PE
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_numWorkGroups = (m_numBodies + m_workGroupSize - 1) / m_workGroupSize;
    setIntegrationType();


    const auto startShader = Shader({{"../src/shaders/compute/startTest.comp", GL_COMPUTE_SHADER}});

    // For setting the start velocity
    startShader.use();


    startShader.setUniform("dt", m_dt);
    startShader.setUniform("G", m_g);
    startShader.setUniform("epsilon", m_softening);

    // Bind the SSBOs for bodies and energies
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboBodies);
    // Dispatch the work‐groups
    glDispatchCompute(m_numWorkGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    Shader::unUse();
}


void NewtonianSimulationHandler::printBodyPositions(int number)
{
    // 1) Bind the SSBO for bodies so we can map it
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboBodies);

    // 2) Map the entire buffer for read‐only access
    Body* gpuData = static_cast<Body*>(
        glMapBufferRange(
            GL_SHADER_STORAGE_BUFFER,
            0,
            sizeof(Body) * m_numBodies,
            GL_MAP_READ_BIT
        )
    );

    if (gpuData == nullptr) {
        std::cerr << "Error: could not map SSBO for reading body positions\n";
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        return;
    }

    // 3) Iterate and print the positions
    for (int i = 0; i < number; ++i) {
        glm::vec3 pos = gpuData[i].position; // .position is a vec4, but we only care about xyz
        std::cout << "Body[" << i << "] = ("
                  << pos.x << ", "
                  << pos.y << ", "
                  << pos.z << ")\n";
    }

    // 4) Unmap and unbind
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}




