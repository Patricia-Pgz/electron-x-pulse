#include "Mesh.h"
#include <glad/glad.h>

namespace gl3 {
    Mesh::Mesh(const std::vector<float> &vertices) : numberOfVertices((vertices.size() + 1) / 3) {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    }

    void Mesh::draw() const {
        // Bind VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(numberOfVertices));
    }

    Mesh::~Mesh() {
        glDeleteBuffers(1, &VBO);
    }
}