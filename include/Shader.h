#pragma once

#include <GL/glew.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



class Shader {
public:
    // Constructor takes a list of (path, stage) pairs.
    // For Example:
    //   Shader({
    //     { "shaders/particle.vert",       GL_VERTEX_SHADER },
    //     { "shaders/particle.frag",       GL_FRAGMENT_SHADER },
    //   });
    //
    // Or for a compute shader:
    //   Shader({
    //     { "shaders/compute_update.comp", GL_COMPUTE_SHADER }
    //   });
    explicit Shader(const std::vector<std::pair<std::string, int>>& shaderPathsAndStages);
    ~Shader();

    // Call this when the shader must be used
    void use() const;

    static void unUse();

    // Gets the shader program id
    unsigned int id() const;

    template<typename T>
    // Sets a uniform
    void setUniform(const std::string& name, const T& value) const
    {
        GLint loc = getUniformLocation(name);

        if constexpr (std::is_same_v<T, bool>) {
            glUniform1i(loc, static_cast<int>(value));
        }
        else if constexpr (std::is_same_v<T, int>) {
            glUniform1i(loc, value);
        }
        else if constexpr (std::is_same_v<T, unsigned int>) {
            glUniform1ui(loc, value);
        }
        else if constexpr (std::is_same_v<T, float>) {
            glUniform1f(loc, value);
        }
        else if constexpr (std::is_same_v<T, double>) {
            glUniform1d(loc, value);
        }

        else if constexpr (std::is_same_v<T, glm::vec2>) {
            glUniform2f(loc, value.x, value.y);
        }
        else if constexpr (std::is_same_v<T, glm::vec3>) {
            glUniform3f(loc, value.x, value.y, value.z);
        }
        else if constexpr (std::is_same_v<T, glm::vec4>) {
            glUniform4f(loc, value.x, value.y, value.z, value.w);
        }

        else if constexpr (std::is_same_v<T, glm::ivec2>) {
            glUniform2i(loc, value.x, value.y);
        }
        else if constexpr (std::is_same_v<T, glm::ivec3>) {
            glUniform3i(loc, value.x, value.y, value.z);
        }
        else if constexpr (std::is_same_v<T, glm::ivec4>) {
            glUniform4i(loc, value.x, value.y, value.z, value.w);
        }

        else if constexpr (std::is_same_v<T, glm::uvec2>) {
            glUniform2ui(loc, value.x, value.y);
        }
        else if constexpr (std::is_same_v<T, glm::uvec3>) {
            glUniform3ui(loc, value.x, value.y, value.z);
        }
        else if constexpr (std::is_same_v<T, glm::uvec4>) {
            glUniform4ui(loc, value.x, value.y, value.z, value.w);
        }

        else if constexpr (std::is_same_v<T, glm::dvec2>) {
            glUniform2d(loc, value.x, value.y);
        }
        else if constexpr (std::is_same_v<T, glm::dvec3>) {
            glUniform3d(loc, value.x, value.y, value.z);
        }
        else if constexpr (std::is_same_v<T, glm::dvec4>) {
            glUniform4d(loc, value.x, value.y, value.z, value.w);
        }

        else if constexpr (std::is_same_v<T, glm::mat2>) {
            glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat3>) {
            glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat4>) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat2x3>) {
            glUniformMatrix2x3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat3x2>) {
            glUniformMatrix3x2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat2x4>) {
            glUniformMatrix2x4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat4x2>) {
            glUniformMatrix4x2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat3x4>) {
            glUniformMatrix3x4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if constexpr (std::is_same_v<T, glm::mat4x3>) {
            glUniformMatrix4x3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }

        // If you treat GLuint as a sampler (texture unit):
        else if constexpr (std::is_same_v<T, GLuint>) {
            glUniform1i(loc, static_cast<GLint>(value));
        }

        else {
            static_assert(sizeof(T) == 0,
                "Shader::set(): Unsupported uniform type");
        }
    }

    // Reload the shader program
    void reload();

private:
    unsigned int m_programID = 0;
    mutable std::unordered_map<std::string,int> m_uniformLocationCache;

    // Read a shader file
    static std::string readFile(const std::string &path);

    // Compile a shader file
    static unsigned int compileShader(const std::string &source, unsigned int stage);


    // Get the location of a uniform constant in the shader
    int getUniformLocation(const std::string &name) const;

    // Create and link the shader program from the provided paths and stages
    static unsigned int createAndLinkProgram(const std::vector<std::pair<std::string, int>>& shaderPathsAndStages);

    // List of shader paths and their corresponding stages
    std::vector<std::pair<std::string, int>> m_shaderPathsAndStages;
};