#pragma once
#include "engine/Game.h"
#include "PlayerInputSystem.h"
#include "states/LevelPlayState.h"

namespace gl3::game
{
    class GameStateManager;

    class Game final : public engine::Game
    {
    public:
        Game(int width, int height, const std::string& title, const glm::vec3& camPos, float camZoom);
        [[nodiscard]] input::PlayerInputSystem* getPlayerInputSystem() const { return player_input_system_; }

    private:
        void start() override;
        void update(GLFWwindow* window) override;

        void registerUiSystems() override;

        GameStateManager* game_state_manager_;
        input::PlayerInputSystem* player_input_system_;

        float unit = 1.f;
    };
}