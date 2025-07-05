#pragma once
#include "EditorSystem.h"
#include "engine/rendering/Texture.h"
#include "engine/Game.h"
#include "engine/ecs/GameEvents.h"
#include "engine/levelloading/Objects.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::engine::editor
{
    constexpr int tilesPerRow = 4;

    struct TileSelectedEvent //TODO evtl in UIEvent header
    {
        GameObject object;
    };

    class EditorUISystem final : public ui::IUISubsystem
    {
    public:
        explicit EditorUISystem(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game),
                                                                editor_system(new EditorSystem(game))
        {
            ecs::EventDispatcher::dispatcher.sink<context::MouseScrollEvent>().connect<&
                EditorUISystem::onMouseScroll>(this);
        };

        ~EditorUISystem() override
        {
            ecs::EventDispatcher::dispatcher.sink<context::MouseScrollEvent>().disconnect<&
                EditorUISystem::onMouseScroll>(this);
        }

        void setActive(const bool setActive) override
        {
            is_active = setActive;
            if (is_active)
            {
                ecs::EventDispatcher::dispatcher.sink<ecs::PlayModeChange>().connect<&
                    EditorUISystem::onPlayModeChange>(this);
            }
            else
            {
                ecs::EventDispatcher::dispatcher.sink<ecs::PlayModeChange>().disconnect<&
                    EditorUISystem::onPlayModeChange>(this);
            }
        }

        void update() override;

    private:
        void onPlayModeChange(const ecs::PlayModeChange& event);
        void deleteEntity() const;

        void onMouseScroll(const context::MouseScrollEvent& event) const;

        void drawGrid(float gridSpacing);

        void DrawTileSelectionPanel();

        void createCustomUI();

        void visualizeTileSetUI(const rendering::Texture& texture, const std::string& name,
                                float tileSize) const;

        void visualizeSingleTextureUI(const rendering::Texture& texture, const std::string& name,
                                      float tileSize) const;

        void highlightSelectedButton(const std::vector<std::string>& buttonIDs);

        bool is_in_play_mode_ = false;
        bool editor_scrolling_active_ = true;
        ImVec2 grid_center = {0.f, 0.f};
        float grid_offset = 0.5f;
        std::unique_ptr<ImVec2> selected_grid_cell = std::make_unique<ImVec2>(0.0f, 0.0f);
        glm::vec2 selected_scale = {1.f, 1.f};
        char tag_input_buffer[128] = "";
        std::string selected_tag = "platform";
        bool is_triangle = false;
        float selected_z_rotation_ = 0.f;
        bool generate_physics_comp = true;
        bool use_color_ = false;
        glm::vec4 selected_color_ = {1.0f, 1.0f, 1.0f, 1.0f};
        EditorSystem* editor_system;
        static constexpr ImGuiWindowFlags flags_ =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize;
    };
}
