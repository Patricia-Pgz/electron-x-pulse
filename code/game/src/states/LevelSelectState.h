#pragma once
#include "engine/stateManagement/GameState.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "engine/userInterface/UISystem.h"

namespace gl3::game::state
{
    /**
     * @class LevelSelectState
     * @brief  Handles de-/activation of LevelSelectUISystem
     */
    class LevelSelectState final : public engine::state::GameState
    {
    public:
        explicit LevelSelectState(Game& game)
            : GameState(game), level_ui(game.getUISystem()->getSubsystem<engine::levelLoading::LevelSelectUISystem>())
        {
        }

        void onEnter() override
        {
            level_ui->setActive(true);
        }

        void onExit() override
        {
            level_ui->setActive(false);
        }

        void update(float dt) override
        {
        };

    private:
        engine::levelLoading::LevelSelectUISystem* level_ui;
    };
}
