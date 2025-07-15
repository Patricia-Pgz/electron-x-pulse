#include "engine/rendering/Shader.h"
#include <fstream>
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
        vertexShader = loadAndCompileShader(GL_VERTEX_SHADER, vertexShaderPath);
        fragmentShader = loadAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);
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
        const auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::setFloat(const std::string& uniformName, const float value) const
    {
        const GLint location = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform1f(location, value);
    }

    void Shader::setFloatArray(const std::string& name, const float* values, int count) const
    {
        const GLint location = glGetUniformLocation(shaderProgram, name.c_str());
        if (location == -1)
        {
            std::cerr << "Warning: uniform '" << name << "' not found or not used.\n";
            return;
        }
        glUniform1fv(location, count, values);
    }

    void Shader::setVec2(const std::string& uniformName, const glm::vec2& value) const
    {
        const GLint location = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setVec3(const std::string& uniformName, const glm::vec3& value) const
    {
        const GLint location = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setVector4(const std::string& uniformName, glm::vec4 vector) const
    {
        const auto uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());
        glUniform4fv(uniformLocation, 1, glm::value_ptr(vector));
    }

    void Shader::setInt(const std::string& name, const int value) const
    {
        const GLint location = glGetUniformLocation(shaderProgram, name.c_str());
        if (location == -1)
        {
            std::cerr << "Warning: uniform '" << name << "' not found or not used in shader." << std::endl;
        }
        glUniform1i(location, value);
    }

    void Shader::use() const
    {
        glUseProgram(shaderProgram);
    }

    Shader::~Shader()
    {
        if (shaderProgram != 0)
        {
            glDeleteProgram(shaderProgram);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            shaderProgram = 0;
            vertexShader = 0;
            fragmentShader = 0;
        }
    }
}
