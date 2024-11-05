#include "Planet.h"

namespace gl3 {
    struct glData {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
    };

    glData getCircleVertices(float radius) {
        glData circleData;
        float x = 0.0f, y = 0.0f, z = 0.0f;
        int n = 37;
        float angle = 90.0f;
        float step = 360.0f / (n - 1);

        int i = 0;
        while(i < n) {
            circleData.vertices.push_back(x);
            circleData.vertices.push_back(y);
            circleData.vertices.push_back(z);

            x = cos(glm::radians(angle - step * (float) i)) * radius;
            y = sin(glm::radians(angle - step * (float) i)) * radius;

            if(i + 1 < n) {
                circleData.indices.push_back(0);
                circleData.indices.push_back(i + 1);
                circleData.indices.push_back(i + 2);
            }
            i++;
        }

        circleData.indices.push_back(0);
        circleData.indices.push_back(n - 1);
        circleData.indices.push_back(1);

        return circleData;
    }

    const glData glCircleData = getCircleVertices(0.075f);

    Planet::Planet(glm::vec3 position, float size, glm::vec4 color) : Entity(
            Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            Mesh(glCircleData.vertices, glCircleData.indices),
            position,
            0,
            glm::vec3(size, size, size),
            color) {
    }
}