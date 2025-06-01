#pragma once
#include "engine/Game.h"
#include <soloud_wav.h>
#include "PlayerInputSystem.h"

namespace gl3::game
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

    class Game final : public engine::Game
    {
    public:
        Game(int width, int height, const std::string& title, const glm::vec3& camPos, float camZoom);
        ~Game() override;

    private:
        void start() override;
        void update(GLFWwindow* window) override;
        void registerUiSystems() override;
        void moveEntitiesScrolling();
        void on_mouse_scroll(engine::context::onMouseScrollEvent& event);
        void onGameStateChange(); //TODO event
        void onPlayerDeath(engine::ecs::PlayerDeath& event);
        void resetComponents();
        void reset();

        input::PlayerInputSystem player_input_system_;
        engine::GameState previousGameState = engine::GameState::Menu;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;

        float groundLevel = -1;
        float groundHeight = 4.0f;

        float unit = 1.f;

        bool isResetting = false;

        std::vector<GameObject> initial_test_game_objects;

        bool loadLevelFromFile = true;
    };
}
