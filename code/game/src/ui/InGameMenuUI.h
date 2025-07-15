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
        };

        void update() override;

        void setActive(const bool setActive) override
        {
            is_active = setActive;
            show_ui = false;
            escape_pressed = false;
            play_mode_before_pause = false;
            play_mode_saved = false;
        }

        void showUI(const bool showUI)
        {
            show_ui = showUI;
        }

    private:
        void DrawInGameUI(const ImGuiViewport* viewport, ImFont* font);
        bool play_mode_before_pause = false;
        bool play_mode_saved = false;
        bool escape_pressed = false;
        bool show_ui = false;
        float volume = 1.0f;
        static constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;
    };
}
