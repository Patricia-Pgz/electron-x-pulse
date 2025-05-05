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
        std::string selectedTag = "undefined";
    };

    struct UINeonColors {
        static constexpr auto windowBgColor = ImVec4(0.36f, 0.09f, 0.45f, 0.9f);

        static constexpr auto softPastelPink = ImVec4(1.0f, 0.7f, 0.85f, 1.0f);
        static constexpr auto pastelNeonViolet = ImVec4(0.7f, 0.6f, 1.0f, 1.0f);
        static constexpr auto pastelNeonViolet2 = ImVec4(0.48f, 0.3f, 1.0f, 1.0f);

        static constexpr auto Cyan   = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
        static constexpr auto Pink   = ImVec4(1.0f, 0.2f, 0.8f, 1.0f);
        static constexpr auto Orange = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        static constexpr auto Green  = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
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
        void highlightSelectedButton(const std::vector<std::string>& buttonIDs);
        ImVec2 grid_center_ = {0.f, 0.f};
        float grid_offset = 0.5f * pixelsPerMeter;
        std::unique_ptr<ImVec2> selected_screen_pixel = nullptr;
        std::string selected_tag_ = "undefined";
    };
}
