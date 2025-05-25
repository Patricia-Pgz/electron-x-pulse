#pragma once
#include "engine/rendering/Texture.h"
#include "engine/userInterface/UISystem.h"
#include "engine/Game.h"

namespace gl3::engine::editor
{
    constexpr int tilesPerRow = 4;

    struct TileSelectedEvent
    {
        const rendering::Texture* texture;
        glm::vec4 uv = {0, 0, 1, 1};
        ImVec2 selectedPixelPositionScreen;
        bool isTriangle = false;
        glm::vec2 selectedScale = {1.f, 1.f};
        std::string selectedTag = "undefined";
    };

    class EditorUISystem : public ui::UISystem
    {
    public:
        explicit EditorUISystem(Game& game) : UISystem(game)
        {
        };
        void updateUI() override;

    private:
        void DrawGrid(float gridSpacing);
        void DrawTileSelectionPanel();
        void createCustomUI();
        void visualizeTileSetUI(const rendering::Texture& texture, const std::string& name,
                                float tileSize) const;
        void visualizeSingleTextureUI(const rendering::Texture& texture, const std::string& name,
                                      float tileSize) const;
        void highlightSelectedButton(const std::vector<std::string>& buttonIDs);
        ImVec2 grid_center = {0.f, 0.f};
        float grid_offset = 0.5f;
        std::unique_ptr<ImVec2> selected_grid_cell = nullptr;
        glm::vec2 selected_scale = {1.f, 1.f};
        char tag_input_buffer[128] = "";
        std::string selected_tag = "undefined";
        bool is_triangle = false;
    };
}
