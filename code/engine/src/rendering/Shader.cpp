#include "engine/rendering/Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "engine/Assets.h"


namespace gl3::engine::rendering
{
    struct glStatusData
    {
        int success;
        const char* shaderName;
        char infoLog[GL_INFO_LOG_LENGTH];
    };

    Shader::Shader(const fs::path& vertexShaderPath, const fs::path& fragmentShaderPath)
    {
        vertex_shader = loadAndCompileShader(GL_VERTEX_SHADER, vertexShaderPath);
        fragment_shader = loadAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        glDetachShader(shader_program, vertex_shader);
        glDetachShader(shader_program, fragment_shader);
    }

    unsigned int Shader::loadAndCompileShader(GLuint shaderType, const fs::path& shaderPath)
    {
        const auto shaderSource = readText(shaderPath);
        const auto source = shaderSource.c_str();
        const auto shaderID = glCreateShader(shaderType);
        glShaderSource(shaderID, 1, &source, nullptr);
        glCompileShader(shaderID);

        glStatusData compilationStatus{};
        compilationStatus.shaderName = shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus.success);
        if (compilationStatus.success == GL_FALSE)
        {
            glGetShaderInfoLog(shaderID, GL_INFO_LOG_LENGTH, nullptr, compilationStatus.infoLog);
            throw std::runtime_error(
                "ERROR: " + std::string(compilationStatus.shaderName) + " shader compilation failed.\n" +
                std::string(compilationStatus.infoLog));
        }

        return shaderID;
    }

    std::string Shader::readText(const fs::path& filePath)
    {
        const std::ifstream sourceFile(resolveAssetPath(filePath));
        std::stringstream buffer;
        buffer << sourceFile.rdbuf();
        return buffer.str();
    }

    void Shader::setMat4(const std::string& uniformName, glm::mat4 matrix) const
    {
        const auto uniformLocation = glGetUniformLocation(shader_program, uniformName.c_str());
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::setFloat(const std::string& uniformName, const float value) const
    {
        const GLint location = glGetUniformLocation(shader_program, uniformName.c_str());
        glUniform1f(location, value);
    }

    void Shader::setFloatArray(const std::string& name, const float* values, int count) const
    {
        const GLint location = glGetUniformLocation(shader_program, name.c_str());
        if (location == -1)
        {
            std::cerr << "Warning: uniform '" << name << "' not found or not used.\n";
            return;
        }
        glUniform1fv(location, count, values);
    }

    void Shader::setVec2(const std::string& uniformName, const glm::vec2& value) const
    {
        const GLint location = glGetUniformLocation(shader_program, uniformName.c_str());
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setVec3(const std::string& uniformName, const glm::vec3& value) const
    {
        const GLint location = glGetUniformLocation(shader_program, uniformName.c_str());
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setVector4(const std::string& uniformName, glm::vec4 vector) const
    {
        const auto uniformLocation = glGetUniformLocation(shader_program, uniformName.c_str());
        glUniform4fv(uniformLocation, 1, glm::value_ptr(vector));
    }

    void Shader::setInt(const std::string& name, const int value) const
    {
        const GLint location = glGetUniformLocation(shader_program, name.c_str());
        if (location == -1)
        {
            std::cerr << "Warning: uniform '" << name << "' not found or not used in shader." << std::endl;
        }
        glUniform1i(location, value);
    }

    void Shader::use() const
    {
        glUseProgram(shader_program);
    }

    Shader::~Shader()
    {
        if (shader_program != 0)
        {
            glDeleteProgram(shader_program);
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
            shader_program = 0;
            vertex_shader = 0;
            fragment_shader = 0;
        }
    }
}
