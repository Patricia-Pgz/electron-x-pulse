#include "engine/rendering/Mesh.h"
#include "glad/glad.h"

namespace gl3::engine::rendering
{
    template <typename T>
    GLuint createBuffer(GLuint bufferType, const std::vector<T>& bufferData)
    {
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(bufferType, buffer);
        glBufferData(bufferType, bufferData.size() * sizeof(T), bufferData.data(), GL_STATIC_DRAW);
        return buffer;
    }

    Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices):
        number_of_indices(indices.size()),
        VBO(createBuffer(GL_ARRAY_BUFFER, vertices)),
        EBO(createBuffer(GL_ELEMENT_ARRAY_BUFFER, indices))
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // Position attribute (3 floats)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // TexCoord attribute (2 floats)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void Mesh::draw() const
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::release()
    {
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
        if (EBO)
            glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }


}
