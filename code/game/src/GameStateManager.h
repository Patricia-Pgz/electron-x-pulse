#pragma once
#include "Game.h"
#include "engine/levelloading/Objects.h"

namespace gl3::game
{
    class GameStateManager
    {
    public:
        explicit GameStateManager(Game& game);
        ~GameStateManager();

        [[nodiscard]] const engine::GameState& getCurrentState() const
        {
            return current_game_state_;
        }

        [[nodiscard]] const engine::GameState& getPreviousState() const
        {
            return previous_game_state_;
        }

    private:
        void onUiInitialized() const;
        void onGameStateChange(const engine::ecs::GameStateChange& newState);

        Game& game_;
        engine::events::Event<engine::ui::UISystem>::handle_t onUIInitHandle;
        engine::GameState current_game_state_ = engine::GameState::None;
        engine::GameState previous_game_state_ = engine::GameState::None;
        std::vector<GameObject> initial_test_game_objects;
        bool is_resetting_ = false;
    };
}
