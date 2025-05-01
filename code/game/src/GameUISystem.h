#pragma once
#include "engine/rendering/Texture.h"
#include "engine/userInterface/UISystem.h"

namespace gl3
{
    constexpr int tilesPerRow = 4;

    struct TileSelectedEvent {
        const engine::rendering::Texture* texture;
        glm::vec4 uv;
        glm::vec2 selectedTilePositionWorld;
    };

    class GameUISystem : public engine::ui::UISystem {
    public:
        explicit GameUISystem(engine::Game& game) : UISystem(game)
        {
        };
        void updateUI() override;
    private:
        void handleGridSelection();
        void DrawGrid(const ImVec2& screenSize, float gridSpacing) const;
        void DrawTileSelectionPanel() const;
        void createCustomUI(const ImVec2& screenSize) const;
        void visualizeTileSetUI(const engine::rendering::Texture& texture, const std::string& name, float tileSize) const;
        void visualizeSingleTextureUI(const engine::rendering::Texture& texture, const std::string& name,
                                      float tileSize) const;
        ImVec2 grid_center_ = {0.f, 0.f};
        float grid_offset = 0.5f;
        glm::vec2 selected_grid_cell_ = {0.f, 0.f};
    };
}



