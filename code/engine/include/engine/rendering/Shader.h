#pragma once

#include <string>
#include <filesystem>
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace fs = std::filesystem;

namespace gl3::engine::rendering
{
    /**
     * @class Shader
     * @brief Manages an OpenGL shader program, including loading, compiling, and setting uniforms.
     *
     * This class loads vertex and fragment shaders from files, compiles them, links them into a program,
     * and provides methods to activate the shader and set uniform variables.
     */
    class Shader
    {
    public:
        /**
         * @brief Constructs and compiles a shader program from vertex and fragment shader source files.
         * @param vertexShaderPath Path to the vertex shader file.
         * @param fragmentShaderPath Path to the fragment shader file.
         */
        Shader(const fs::path& vertexShaderPath, const fs::path& fragmentShaderPath);

        /**
         * @brief Destructor that deletes the shader program and its shaders.
         */
        ~Shader();

        // Delete copy constructor
        Shader(const Shader& shader) = delete;

        /**
         * @brief Move constructor.
         * @param other Shader to move from.
         */
        Shader(Shader&& other) noexcept
        {
            std::swap(this->shader_program, other.shader_program);
            std::swap(this->vertex_shader, other.vertex_shader);
            std::swap(this->fragment_shader, other.fragment_shader);
        }

        /**
         * @brief Set a mat4 uniform variable in the shader.
         * @param uniformName Name of the uniform in the shader.
         * @param matrix The matrix to set.
         */
        void setMat4(const std::string& uniformName, glm::mat4 matrix) const;

        /**
         * @brief Set a float uniform variable in the shader.
         * @param uniformName Name of the uniform.
         * @param value Value to set.
         */
        void setFloat(const std::string& uniformName, float value) const;

        /**
         * @brief Set an array of floats as a uniform.
         * @param name Name of the uniform.
         * @param values Pointer to array of floats.
         * @param count Number of floats.
         */
        void setFloatArray(const std::string& name, const float* values, int count) const;

        /**
         * @brief Set a vec2 uniform variable.
         * @param uniformName Name of the uniform.
         * @param value 2D vector value.
         */
        void setVec2(const std::string& uniformName, const glm::vec2& value) const;

        /**
         * @brief Set a vec3 uniform variable.
         * @param name Name of the uniform.
         * @param value 3D vector value.
         */
        void setVec3(const std::string& name, const glm::vec3& value) const;

        /**
         * @brief Set a vec4 uniform variable.
         * @param uniformName Name of the uniform.
         * @param vector 4D vector value.
         */
        void setVector4(const std::string& uniformName, glm::vec4 vector) const;

        /**
         * @brief Set an int uniform variable.
         * @param name Name of the uniform.
         * @param value Integer value.
         */
        void setInt(const std::string& name, int value) const;

        /**
         * @brief Activate this shader program for rendering.
         */
        void use() const;

    private:
        /**
         * @brief Load and compile a single shader stage.
         * @param shaderType OpenGL shader type (e.g., GL_VERTEX_SHADER).
         * @param shaderPath Path to the shader source file.
         * @return OpenGL shader ID.
         */
        unsigned int loadAndCompileShader(GLuint shaderType, const fs::path& shaderPath);

        /**
         * @brief Read a text file into a string.
         * @param filePath Path to the text file.
         * @return File contents as a string.
         */
        static std::string readText(const fs::path& filePath);

        unsigned int shader_program = 0;     ///< OpenGL shader program ID.
        unsigned int vertex_shader = 0;      ///< Vertex shader ID.
        unsigned int fragment_shader = 0;    ///< Fragment shader ID.
    };
}
