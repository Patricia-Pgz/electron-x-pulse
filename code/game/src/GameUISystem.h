#pragma once
#include "Constants.h"
#include "engine/rendering/Texture.h"
#include "engine/userInterface/UISystem.h"

namespace gl3
{
    constexpr int tilesPerRow = 4;

    struct TileSelectedEvent
    {
        const engine::rendering::Texture* texture;
        glm::vec4 uv;
        ImVec2 selectedPixelPositionScreen;
    };

    class GameUISystem : public engine::ui::UISystem
    {
    public:
        explicit GameUISystem(engine::Game& game) : UISystem(game)
        {
        };
        void updateUI() override;

    private:
        void DrawGrid( float gridSpacing);
        void DrawTileSelectionPanel();
        void createCustomUI() ;
        void visualizeTileSetUI(const engine::rendering::Texture& texture, const std::string& name,
                                float tileSize) const;
        void visualizeSingleTextureUI(const engine::rendering::Texture& texture, const std::string& name,
                                      float tileSize) const;
        ImVec2 grid_center_ = {0.f, 0.f};
        float grid_offset = 0.5f * pixelsPerMeter;
        std::unique_ptr<ImVec2> selected_screen_pixel = nullptr;
        std::string selected_image_button_id;
    };
}
