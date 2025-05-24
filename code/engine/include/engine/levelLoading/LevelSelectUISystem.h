#pragma once
#include "engine/userInterface/UISystem.h"

namespace gl3::engine::levelLoading
{
    class LevelSelectUISystem : public ui::UISystem
    {
    public:
        explicit LevelSelectUISystem(Game& game) : UISystem(game)
        {
        };
        void updateUI() override;

    private:
        void createLevelSelection();
    };
}
