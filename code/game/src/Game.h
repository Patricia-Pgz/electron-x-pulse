#pragma once
#include "engine/Game.h"
#include <soloud_wav.h>
#include "PlayerInputSystem.h"

namespace gl3::game
{
    class GameStateManager;

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

        GameStateManager* game_state_manager_;
        input::PlayerInputSystem player_input_system_;

        float unit = 1.f;
        bool loadLevelFromFile = true;
    };
}
