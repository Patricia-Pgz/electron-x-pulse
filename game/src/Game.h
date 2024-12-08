#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soloud.h>
#include <soloud_wav.h>
#include "entities/Ship.h"
#include <box2d/box2d.h>

namespace gl3 {
    class Game {
    public:
        Game(int width, int height, const std::string &title);
        virtual ~Game();
        void run();
        glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);
        [[nodiscard]] Ship *getShip() const { return ship; }
        GLFWwindow *getWindow() const { return window; }
        b2WorldId getPhysicsWorld() const;

    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        void update();
        void draw();
        void updateDeltaTime();
        void updatePhysics();
        void resetEntities();
        void reset();

        GLFWwindow *window = nullptr;
        std::vector<std::unique_ptr<Entity>> entities;
        Ship *ship = nullptr;
        SoLoud::Soloud audio;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        float groundLevel = -1;
        float lastFrameTime = 1.0f/60;
        float deltaTime = 1.0f/60;
        b2WorldId physicsWorld;
        float accumulator = 0.f;
    };
}



