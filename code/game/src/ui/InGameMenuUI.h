#pragma once
#include "engine/Game.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::game::ui
{
    class InGameMenuUI final : public engine::ui::IUISubsystem
    {
    public:
        explicit InGameMenuUI(ImGuiIO* imguiIO, engine::Game& game) : IUISubsystem(imguiIO, game)
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::EditorPlayModeChange>().connect<&
                InGameMenuUI::onPlayModeChange>(this);
        };

        ~InGameMenuUI() override
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::EditorPlayModeChange>().disconnect<&
                InGameMenuUI::onPlayModeChange>(this);
        }

        void update() override;

        void setActive(const bool setActive) override
        {
            is_active = setActive;
            show_ui = false;
            escape_pressed = false;
        }

        void setEditMode(bool editMode)
        {
            is_edit_mode = editMode;
        }

        void showUI(const bool showUI)
        {
            show_ui = showUI;
        }

    private:
        void onPlayModeChange(const engine::ecs::EditorPlayModeChange& event);
        void DrawInGameUI(const ImGuiViewport* viewport, ImFont* font);
        bool is_edit_mode = false;
        bool is_play_mode = false;
        bool escape_pressed = false;
        bool show_ui = false;
        float volume = 1.0f;
        static constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;
    };
}
