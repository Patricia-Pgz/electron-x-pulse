#pragma once
#include "engine/userInterface/UISystem.h"

namespace gl3
{
    class GameUISystem : public engine::ui::UISystem {
    public:
        explicit GameUISystem(engine::Game& game) : UISystem(game)
        {
        };
        void updateUI() override;
    private:
        void DrawGrid(float gridSpacing);
        void DrawTileSelectionPanel() const;
        void createCustomUI();
    };
}



