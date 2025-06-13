#pragma once
#include "EditorSystem.h"
#include "engine/rendering/Texture.h"
#include "engine/Game.h"
#include "engine/levelloading/Objects.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::engine::editor
{
    constexpr int tilesPerRow = 4;

    struct TileSelectedEvent //TODO evtl in UIEvent header
    {
        GameObject object;
    };

    class EditorUISystem : public ui::IUISubsystem
    {
    public:
        explicit EditorUISystem(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game), editor_system(new EditorSystem(game))
        {
        };
        void update() override;

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
        EditorSystem* editor_system;
    };
}
