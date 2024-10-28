#pragma once
#include <string>
#include "glad/glad.h"
#include <glm/glm.hpp>

namespace gl3 {

class Shader {
public:
    Shader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource);
    virtual ~Shader();
    void setMatrix(const std::string &uniformName, glm::mat4 matrix) const;
    void setVector(const std::string &uniformName, glm::vec4 vector) const;
    void use() const;

private:
    unsigned int shaderProgram = 0;
    unsigned int vertexShader = 0;
    unsigned int fragmentShader = 0;
    unsigned int loadAndCompileShader(GLuint shaderType, const std::string &shaderSource);
};

}