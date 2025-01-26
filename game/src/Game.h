#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soloud.h>
#include <soloud_wav.h>
#include "entities/Player.h"
#include <box2d/box2d.h>

namespace gl3 {

    enum class GameState {
        Menu,
        Level,
        PreviewWithScrolling,
        PreviewWithTesting
    };

    class Game
    {
    public:
        Game(int width, int height, const std::string &title, glm::vec3 camPos, float camZoom);
        virtual ~Game();
        void run();
        glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);
        [[nodiscard]] Player *getShip() const { return player; }
        GLFWwindow *getWindow() const { return window; }
        b2WorldId getPhysicsWorld() const;
        void scroll_callback_fun(double yOffset);
        float bpm = 0.0f; // Default BPM, updated dynamically //TODO not public



    private:
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        void update();
        void draw();
        void updateDeltaTime();
        void updatePhysics();
        void drawTimeline();
        void calculateWindowBounds();
        bool isInVisibleWindow(const b2Vec2& position) const;
        void resetEntities();
        void setCameraPosition(const glm::vec3& position);
        void setZoom(float newZoom);
        void reset();


        GLFWwindow *window = nullptr;
        glm::vec3 cameraPosition;
        glm::vec3 cameraCenter {0.0f, 0.0f, 0.0f};
        float zoom;
        float windowLeft, windowRight, windowBottom, windowTop;

        GameState currentGameState = GameState::PreviewWithTesting;
        std::vector<std::unique_ptr<Entity>> entities;
        Player *player = nullptr;
        SoLoud::Soloud audio;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        float levelLength = 0;
        float groundLevel = -1;
        float lastFrameTime = 1.0f/60;
        float deltaTime = 1.0f/60;
        b2WorldId physicsWorld;
        float accumulator = 0.f;

        float distancePerBeat = 1.0f; // Example: player travels 1 unit per beat
        float scrollSpeed = -1.0f;
        bool isAudioPlaying = false;
        float initialPlayerPositionX = -2;
    };
}



