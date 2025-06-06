//
// Created by dorito009 on 6/3/25.
//

#ifndef NEWTONIANSIMULATIONHANDLER_H
#define NEWTONIANSIMULATIONHANDLER_H
#include <vector>
#include <glm/vec4.hpp>

#include "Shader.h"

static std::string EULER_PATH = "../src/shaders/compute/euler.comp";
static std::string LEAPFROG1_PATH = "../src/shaders/compute/leapfrog1.comp";
static std::string LEAPFROG2_PATH = "../src/shaders/compute/leapfrog2.comp";

enum IntegrationType {
    EULER,
    LEAPFROG
};

struct Body {
    glm::vec4 position;
    glm::vec4 impulse; // impulse.xyz is velocity .w is mass
    glm::vec4 acceleration;

    Body(const glm::vec4& position, const glm::vec4& impulse, const glm::vec4& acceleration = glm::vec4(0.0f)):
        position(position), impulse(impulse), acceleration(acceleration) {}
};

struct Energy {
    float kineticEnergy; // Kinetic energy
    float potentialEnergy; // Potential energy
    [[nodiscard]] float totalEnergy() const {
        return kineticEnergy + potentialEnergy;
    }
    Energy(float ke, float pe) : kineticEnergy(ke), potentialEnergy(pe) {}
};



class NewtonianSimulationHandler {
public:
    NewtonianSimulationHandler(int numBodies, IntegrationType integrationType);

    ~NewtonianSimulationHandler();

    // Starts a new simulation with the specified number of bodies and integration type
    void newSimulation(int numBodies, IntegrationType integrationType);

    // Attempts to step the simulation forward by one time step
    bool tryStepSimulation();

    GLuint getBodySSBO() {
        return m_ssboBodies;
    }

    std::vector<float> getTotalEnergy() const {
        std::vector<float> totalEnergies;
        totalEnergies.reserve(m_energies.size());
        if (m_energies.size() < 2) {
            return totalEnergies;
        }
        const float e_0 = m_energies[1].totalEnergy();
        for (const auto& energy : m_energies) {
            totalEnergies.push_back(-energy.totalEnergy() / (e_0 + 0.00001f));
        }
        return totalEnergies;
    }

    int getNumBodies() const {
        return m_numBodies;
    }
    void printBodyPositions(int number);
    
private:

    int m_numBodies; // Number of bodies in the simulation
    std::vector<Body> m_bodies; // Host-side storage for bodies
    std::vector<Energy> m_energies; // Host-side storage for energies

    IntegrationType m_integrationType; // Type of integration method used (Euler or Leapfrog)

    Shader m_eulerComputeShader; // Compute shader for the euler simulation

    Shader m_leapfrogComputeShader1; // First compute shader for the leapfrog simulation
    Shader m_leapfrogComputeShader2; // Second compute shader for the leapfrog simulation

    GLuint m_ssboBodies = 0; // Shader Storage Buffer Object for bodies
    GLuint m_ssboEnergies = 0; // Shader Storage Buffer Object for energies

    int m_workGroupSize = 256; // must match 'layout(local_size_x=256)' in GLSL
    int m_numWorkGroups = 0; // Number of work groups to dispatch, computed as ceil(numBodies / workGroupSize)

    bool m_running = false; // If true, the simulation is running

    float m_g = 0.01f; // Gravitational constant
    float m_dt = 10.1f; // Time step for simulation in seconds
    float m_softening = 0.1f; // Softening factor to prevent singularities

    // Initializes the bodies
    void initiateBodies();
    // Sets the integration type for the compute shaders
    void setIntegrationType() const;
    // Sets up the compute shaders and SSBOs
    void setupShaders();

    // Computes the energy of the system
    void computeEnergy();
};



#endif //NEWTONIANSIMULATIONHANDLER_H
