#pragma once
#include <vector>

namespace gl3 {
        class Mesh {
        public:
                explicit Mesh(const std::vector<float> &vertices);
                void draw() const;
                virtual ~Mesh();

        private:
                unsigned int VBO = 0;
                unsigned int numberOfVertices;
        };
}
