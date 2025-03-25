#pragma once
#include "engine/Game.h"
#include <soloud_wav.h>
#include "entities/Player.h"
#include "physics/PhysicsSystem.h"

namespace gl3
{
    struct GameObject
    {
        float positionX;
        float positionY;
        bool isPlatform;
        float scaleY;
        float scaleX;
        glm::vec4 color;
    };

    class Game : public engine::Game
    {
    public:
        Game(int width, int height, const std::string& title, glm::vec3 camPos, float camZoom);
        ~Game() override;
        [[nodiscard]] const entt::entity& getPlayer() const { return *player; }
        float bpm = 0.0f; // Default BPM, updated dynamically //TODO not public
        float distancePerBeat = 2.0f; // Example: player travels 1 unit per beat

    private:
        void start() override;
        void update(GLFWwindow* window) override;
        void draw() override;
        void updatePhysics() override;
        void moveEntitiesScrolling();
        void scroll_callback_fun(double yOffset);
        void onGameStateChange(); //TODO event
        void resetEntities();
        void reset();

        entt::registry registry_;
        PhysicsSystem physics_system_;
        engine::GameState previousGameState = engine::GameState::Menu;
        std::unique_ptr<entt::entity> player = nullptr;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        float levelLength = 0;
        float groundLevel = -1;

        bool loadLevelFromFile = true;

        float scrollSpeed = -1.f;
        bool isAudioPlaying = false;
        float initialPlayerPositionX = -2;
    };
}
