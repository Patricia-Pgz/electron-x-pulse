#pragma once
#include <filesystem>
#include <vector>

#include "glad/glad.h"

namespace gl3::engine::rendering
{
    /**
     * @class Mesh
     * @brief Represents a simple GPU mesh with vertex and index buffers.
     *
     * Encapsulates OpenGL VAO/VBO/EBO creation and rendering logic.
     * Manages the lifetime of GPU resources to ensure proper cleanup.
     */
    class Mesh
    {
    public:
        /**
       * @brief Constructs a mesh and uploads vertex and index data to the GPU.
       *
       * @param vertices A flat list of vertex data (positions, uvs).
       * @param indices Indices for indexed drawing of the mesh.
       */
        explicit Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

        /**
         * @brief Draws the mesh using the currently active shader and bound pipeline state.
         */
        void draw() const;

        /**
         * @brief Destructor. Ensures GPU buffers are properly released.
         */
        ~Mesh()
        {
            release();
        }

        /**
         * @brief Deleted copy constructor. Meshes cannot be copied.
         */
        Mesh(const Mesh& other) = delete;

        /**
         * @brief Move constructor.
         *
         * Transfers ownership of the GPU resources to another Mesh instance.
         *
         * @param other The Mesh to move from.
         */
        Mesh(Mesh&& other) noexcept
        {
            std::swap(this->VAO, other.VAO);
            std::swap(this->VBO, other.VBO);
            std::swap(this->EBO, other.EBO);
            std::swap(this->number_of_indices, other.number_of_indices);
        }

        /**
         * @brief Releases the GPU resources used by this mesh (VAO, VBO, EBO).
         */
        void release();

    private:
        unsigned int VAO = 0;  ///< OpenGL Vertex Array Object.
        unsigned int VBO = 0;  ///< OpenGL Vertex Buffer Object.
        unsigned int EBO = 0;  ///< OpenGL Element Buffer Object.
        unsigned int number_of_indices = 0; ///< Number of indices used for drawing.
    };
}
