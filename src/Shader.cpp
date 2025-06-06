#include "../include/src/Shader.h"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const std::vector<std::pair<std::string, int> > &shaderPathsAndStages) : m_shaderPathsAndStages(shaderPathsAndStages) {
    m_programID = createAndLinkProgram(shaderPathsAndStages);
}

Shader::~Shader()
{
    if (m_programID) {
        glDeleteProgram(m_programID);
    }
}

void Shader::reload() {
    // Delete the old program
    if (m_programID) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
    m_uniformLocationCache.clear();

    m_programID = createAndLinkProgram(m_shaderPathsAndStages);
}


void Shader::use() const
{
    glUseProgram(m_programID);
}

void Shader::unUse() {
    glUseProgram(0);
}

unsigned int Shader::id() const
{
    return m_programID;
}

std::string Shader::readFile(const std::string& path) {
    // Open the file
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return "";
    }

    // read the file and write to a string
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

unsigned int Shader::compileShader(const std::string& source, const unsigned int stage) {

    // Create shader
    const unsigned int shader = glCreateShader(stage);
    const char* src = source.c_str();

    // Compile the shader
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compile errors
    int success;
    char infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::"
                  << (stage == GL_VERTEX_SHADER   ? "VERTEX"   :
                      stage == GL_FRAGMENT_SHADER ? "FRAGMENT" :
                      stage == GL_GEOMETRY_SHADER ? "GEOMETRY" :
                      stage == GL_COMPUTE_SHADER  ? "COMPUTE"  :
                      stage == GL_TESS_CONTROL_SHADER     ? "TESS_CONTROL"     :
                      stage == GL_TESS_EVALUATION_SHADER  ? "TESS_EVALUATION"  :
                                                         "UNKNOWN")
                  << "::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    return shader;
}

int Shader::getUniformLocation(const std::string& name) const {
    // Check if we already asked for the location
    if (m_uniformLocationCache.count(name))
        return m_uniformLocationCache.at(name);

    // Ask the location
    const int location = glGetUniformLocation(m_programID, name.c_str());
    if (location == -1) {
        std::cerr << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << m_shaderPathsAndStages[0].first<< std::endl;
    }

    // Save the location and return it
    m_uniformLocationCache[name] = location;
    return location;
}


unsigned int Shader::createAndLinkProgram(const std::vector<std::pair<std::string, int>>& shaderPathsAndStages) {
    // Save and reserve the ids for the shaders
    std::vector<unsigned int> shaderIDs;
    shaderIDs.reserve(shaderPathsAndStages.size());


    for (const auto& [path, stage] : shaderPathsAndStages) {
        // Read sources from disk
        std::string source = readFile(path);
        if (source.empty()) {
            std::cerr << "ERROR::SHADER::SOURCE_EMPTY for " << path << std::endl;
            continue;
        }

        // Compile the shader

        unsigned int id = compileShader(source, stage);
        shaderIDs.push_back(id);
    }


    // Create Program and attach all the shaders
    unsigned int programID = glCreateProgram();
    for (const unsigned int shaderId : shaderIDs) {
        glAttachShader(programID, shaderId);
    }

    // Link the program
    glLinkProgram(programID);

    // Check for linking errors
    int success;
    char infoLog[1024];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete individual shaders, since they’re now linked
    for (const unsigned int shaderId : shaderIDs) {
        glDeleteShader(shaderId);
    }

    return programID;
}







