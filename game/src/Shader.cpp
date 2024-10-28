#include "Shader.h"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

namespace gl3 {
    struct glStatusData {
        int success;
        const char *shaderName;
        char infoLog[GL_INFO_LOG_LENGTH];
    };

    Shader::Shader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource)
    {
        vertexShader = loadAndCompileShader(GL_VERTEX_SHADER,vertexShaderSource);
        fragmentShader = loadAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        // Shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);
    }

    unsigned int Shader::loadAndCompileShader(GLuint shaderType, const std::string &shaderSource) {
        auto shaderID = glCreateShader(shaderType);
        auto source = shaderSource.c_str();
        glShaderSource(shaderID, 1, &source, nullptr);
        glCompileShader(shaderID);

        glStatusData compilationStatus{};
        compilationStatus.shaderName = shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus.success);
        if(compilationStatus.success == GL_FALSE) {
            glGetShaderInfoLog(shaderID, GL_INFO_LOG_LENGTH, nullptr, compilationStatus.infoLog);
            throw std::runtime_error("ERROR: " + std::string(compilationStatus.shaderName) + " shader compilation failed.\n" +
            std::string(compilationStatus.infoLog));
        }

        return shaderID;
    }
    void Shader::setMatrix(const std::string &uniformName, glm::mat4 matrix) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::setVector(const std::string &uniformName, glm::vec4 vector) const {
        auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform4fv(uniformLocation, 1, glm::value_ptr(vector));
    }
    void Shader::use() const
    {
        glUseProgram(shaderProgram);
    }

    Shader::~Shader()
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }



} // gl3