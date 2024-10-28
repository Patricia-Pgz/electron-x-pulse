#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"

namespace gl3 {
    class Game {
    public:
        Game(int width, int height, const std::string &title);
        virtual ~Game();
        void run();

    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        void update();
        void draw();
        void updateDeltaTime();
        glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);

        GLFWwindow *window = nullptr;
        glm::mat4 mvpMatrix;
        Shader *shader = nullptr;
        Mesh *mesh = nullptr;

        const char *vertexShaderSource = R"(
            #version 460 core
            layout (location = 0) in vec3 aPos;
            uniform mat4 mvp;
            void main() {
                gl_Position = mvp * vec4(aPos.xyz, 1.0);
            }
        )";

        const char *fragmentShaderSource = R"(
            #version 460 core
            out vec4 fragColor;
            void main() {
                fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        )";

        float zRotation = 0.0f;
        float rotationSpeed = 120.0f;
        float xTranslate = 0.0f;
        float yTranslate = 0.0f;
        float translationSpeed = 1.0f;

        float lastFrameTime = 1.0f/60;
        float deltaTime = 1.0f/60;
    };
}



