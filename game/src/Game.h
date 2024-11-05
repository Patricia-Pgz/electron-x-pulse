#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "entities/Ship.h"

namespace gl3 {
    class Game {
    public:
        Game(int width, int height, const std::string &title);
        virtual ~Game();
        void run();
        glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);
        Ship *getShip() { return ship; }
        GLFWwindow *getWindow() { return window; }

    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        void update();
        void draw();
        void updateDeltaTime();

        GLFWwindow *window = nullptr;
        std::vector<std::unique_ptr<Entity>> entities;
        Ship *ship = nullptr;
        float lastFrameTime = 1.0f/60;
        float deltaTime = 1.0f/60;
        SoLoud::Soloud audio;
        SoLoud::Wav backGroundSound;
    };
}



