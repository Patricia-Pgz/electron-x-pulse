#pragma once
#include "engine/stateManagement/GameState.h"
#include "engine/levelLoading/LevelSelectUISystem.h"

namespace gl3::game::state
{
    class LevelSelectState final : public engine::state::GameState
    {
    public:
        explicit LevelSelectState(Game& game)
            : GameState(game), level_ui_(game.getUISystem()->getSubsystem<engine::levelLoading::LevelSelectUISystem>())
        {
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
        engine::levelLoading::LevelSelectUISystem* level_ui_;
    };
}
