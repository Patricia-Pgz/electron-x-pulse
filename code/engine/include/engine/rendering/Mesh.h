#pragma once
#include <filesystem>
#include <vector>

#include "glad/glad.h"

namespace gl3::engine::rendering
{
    class Mesh
    {
    public:
        explicit Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        void draw() const;

        ~Mesh()
        {
            release();
        };

        // Delete copy constructor
        Mesh(const Mesh& other) = delete;

        // Explicit move constructor
        Mesh(Mesh&& other) noexcept
        {
            std::swap(this->VAO, other.VAO);
            std::swap(this->VBO, other.VBO);
            std::swap(this->EBO, other.EBO);
            std::swap(this->numberOfIndices, other.numberOfIndices);
        }

        void release()
        {
            if (VAO)
                glDeleteVertexArrays(1, &VAO);
            if (VBO)
                glDeleteBuffers(1, &VBO);
            if (EBO)
                glDeleteBuffers(1, &EBO);
            VAO = VBO = EBO = 0;
        }

    private:
        unsigned int VAO = 0;
        unsigned int VBO = 0;
        unsigned int EBO = 0;
        unsigned int numberOfIndices = 0;
    };
}
