#pragma once
#include "engine/Game.h"
#include <soloud_wav.h>

#include "PlayerInputSystem.h"
#include "engine/rendering/RenderingSystem.h"
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
        entt::entity entityID;
    };

    class Game : public engine::Game
    {
    public:
        Game(int width, int height, const std::string& title, const glm::vec3& camPos, float camZoom);

        ~Game() override;

    private:
        void start() override;
        void update(GLFWwindow* window) override;
        void draw() override;
        void updatePhysics() override;
        void moveEntitiesScrolling();
        void scroll_callback_fun(double yOffset);
        void onGameStateChange(); //TODO event
        void onPlayerDeath(engine::ecs::PlayerDeath& event);
        void resetComponents();
        void reset();

        PhysicsSystem physics_system_;
        engine::rendering::RenderingSystem rendering_system_;
        PlayerInputSystem player_input_system_;
        engine::GameState previousGameState = engine::GameState::Menu;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;

        float levelLength = 0;
        float groundLevel = -1;
        float groundHeight = 4.0f;

        bool isResetting = false;

        std::vector<GameObject> initial_test_game_objects;

        bool loadLevelFromFile = true;

        float initialPlayerPositionX = -2;
    };
}
