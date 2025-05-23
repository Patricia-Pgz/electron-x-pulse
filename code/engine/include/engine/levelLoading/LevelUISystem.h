#pragma once
#include "engine/userInterface/UISystem.h"

namespace gl3::engine::levelLoading
{
    class LevelUISystem : public ui::UISystem
    {    public:
        explicit LevelUISystem(Game& game) : UISystem(game)
        {
        };
        void updateUI() override;

        private:
        void createLevelSelection();

    };
}
