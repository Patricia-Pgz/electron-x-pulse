#pragma once
#include "Game.h"

namespace gl3::game
{
    class GameStateManager
    {
    public:
        explicit GameStateManager(Game& game);
        ~GameStateManager();

    private:
        void onGameStateChange(engine::ecs::GameStateChange newState);
        void resetComponents();
        void reset();
        void onPlayerDeath(engine::ecs::PlayerDeath& event);
        Game& game_;
        engine::GameState current_game_state_ = engine::GameState::None;
        engine::GameState previous_game_state_ = engine::GameState::None;
        std::vector<GameObject> initial_test_game_objects;
        bool is_resetting_;
    };
}
