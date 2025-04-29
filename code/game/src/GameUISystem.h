#pragma once
#include "engine/rendering/Texture.h"
#include "engine/userInterface/UISystem.h"

namespace gl3
{
    struct TileSelectedEvent {
        const engine::rendering::Texture* texture;
        glm::vec4 uv;

    };

    class GameUISystem : public engine::ui::UISystem {
    public:
        explicit GameUISystem(engine::Game& game) : UISystem(game)
        {
        };
        void updateUI() override;
    private:
        static void DrawGrid(float gridSpacing);
        void DrawTileSelectionPanel() const;
        void createCustomUI() const;

    };
}



