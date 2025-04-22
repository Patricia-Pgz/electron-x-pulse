#pragma once
#include <filesystem>
#include <vector>

namespace gl3::engine::rendering {
    class Mesh {
    public:
        explicit Mesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices);
        void draw() const;
        ~Mesh();

        // Delete copy constructor
        Mesh(const Mesh &other) = delete;

        // Explicit move constructor
        Mesh(Mesh &&other) noexcept {
            std::swap(this->VAO, other.VAO);
            std::swap(this->VBO, other.VBO);
            std::swap(this->EBO, other.EBO);
            std::swap(this->numberOfIndices, other.numberOfIndices);
        }

    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        unsigned int numberOfIndices = 0;
    };
}
