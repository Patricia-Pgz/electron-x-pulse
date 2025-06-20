#pragma once
#include "engine/stateManagement/GameState.h"
#include "engine/levelLoading/LevelSelectUISystem.h"

namespace gl3::engine::state
{
    class LevelSelectState final : public GameState
    {
    public:
        explicit LevelSelectState(Game& game)
            : GameState(game)
        {
            level_ui_ = game.getUISystem().getSubsystem<levelLoading::LevelSelectUISystem>();
        }

        void onEnter() override
        {
            level_ui_->setActive(true);
        }

        void onExit() override
        {
            level_ui_->setActive(false);
        }

        void update(float dt) override
        {
        };

    private:
        levelLoading::LevelSelectUISystem* level_ui_;
    };
}

