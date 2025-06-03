#ifndef SIMULATIONHANDLER_H
#define SIMULATIONHANDLER_H
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Shader.h"

struct Body {
    const glm::vec4 position;
    // impulse.xyz is velocity .w is mass
    const glm::vec4 impulse;

    Body(const glm::vec4& position, const glm::vec4& impulse) :
        position(position), impulse(impulse) {}
};



class SimulationHandler {
public:
    SimulationHandler(int numBodies);

    void step() const;

    GLuint getBoddySSBO() const {
        return m_ssboBodies;
    };

private:
    int m_numBodies;
    std::vector<Body> hostBodies;
    Shader m_computeShader;

    GLuint m_ssboBodies = 0;        // will hold all Body structs (vec4+vec4)

    int m_workGroupSize = 256;      // must match 'layout(local_size_x=256)' in GLSL
    int m_numWorkGroups = 0;        // computed as ceil(numBodies / workGroupSize)

    float m_dt        = 1;
    float m_G         = 1;
    float m_softening = 1;


    std::pair<glm::vec3, glm::vec3> randomPointAndVelocityInGalaxy();
};



#endif //SIMULATIONHANDLER_H
