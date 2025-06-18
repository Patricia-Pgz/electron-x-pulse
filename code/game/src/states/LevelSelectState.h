#pragma once
#include "engine/stateManagement/GameState.h"
#include "engine/levelLoading/LevelSelectUISystem.h"

namespace gl3::engine::state
{
    class LevelSelectState : public GameState
    {
    public:
        explicit LevelSelectState(levelLoading::LevelSelectUISystem& levelUI)
            : level_ui_(levelUI)
        {
        }

        void onEnter() override
        {
            level_ui_.setActive(true);
        }

        void onExit() override
        {
            level_ui_.setActive(false);
        }

        void update(float dt) override
        {
        };

    private:
        levelLoading::LevelSelectUISystem& level_ui_;
    };
}

