#pragma once
#include <string>
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <filesystem>

namespace fs = std::filesystem;

namespace gl3 {

class Shader {
public:
    Shader(const fs::path &vertexShaderPath, const fs::path &fragmentShaderPath);
    virtual ~Shader();
    void setMatrix(const std::string &uniformName, glm::mat4 matrix) const;
    void setVector(const std::string &uniformName, glm::vec4 vector) const;
    void use() const;
    // Delete copy constructor
    Shader(const Shader &shader) = delete;

    // Explicit move constructor
    Shader(Shader &&other) noexcept {
        std::swap(this->shaderProgram, other.shaderProgram);
        std::swap(this->vertexShader, other.vertexShader);
        std::swap(this->fragmentShader, other.fragmentShader);
    }

private:
    unsigned int shaderProgram = 0;
    unsigned int vertexShader = 0;
    unsigned int fragmentShader = 0;
    unsigned int loadAndCompileShader(GLuint shaderType, const fs::path &shaderPath);
};

}