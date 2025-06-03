#ifndef SIMULATIONHANDLER_H
#define SIMULATIONHANDLER_H
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../include/src/Shader.h"

struct Body {
    glm::vec4 position;
    // impulse.xyz is velocity .w is mass
    glm::vec4 impulse;

    Body(const glm::vec4& position, const glm::vec4& impulse) :
        position(position), impulse(impulse) {}
};



class SimulationHandler {
public:
    SimulationHandler(int numBodies);

    ~SimulationHandler();

    void step();
    void newSimulation(int bodyCount);

    GLuint getBoddySSBO() const {
        return m_ssboBodies;
    };
    std::vector<float> getEnergy() const {
        return m_energy;
    }

private:
    int m_numBodies;
    std::vector<Body> hostBodies;
    Shader m_computeShader;

    GLuint m_ssboBodies = 0;
    GLuint m_ssboEnergy = 0;

    int m_workGroupSize = 256;      // must match 'layout(local_size_x=256)' in GLSL
    int m_numWorkGroups = 0;        // computed as ceil(numBodies / workGroupSize)

    float m_dt        = 1;
    float m_G         = 1;
    float m_softening = 1;


    std::pair<glm::vec3, glm::vec3> randomPointAndVelocityInGalaxy();

    std::pair<float, float> computeEnergiesFromGPU() const;

    std::vector<float> m_energy;

};



#endif //SIMULATIONHANDLER_H
