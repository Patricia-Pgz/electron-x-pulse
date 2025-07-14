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
            return current_game_state;
        }

        [[nodiscard]] const engine::GameState& getPreviousState() const
        {
            return previous_game_state;
        }

    private:
        void onUiInitialized() const;
        void onEditModeChange(const engine::ui::EditModeButtonPress& event);
        void onGameStateChange(const engine::ecs::GameStateChange& newState);

        Game& game;
        engine::events::Event<engine::ui::UISystem>::handle_t onUIInitHandle;
        engine::GameState current_game_state = engine::GameState::None;
        engine::GameState previous_game_state = engine::GameState::None;
        std::vector<GameObject> initial_test_game_objects;
        bool is_resetting = false;
        bool is_edit_mode = false;
    };
}
