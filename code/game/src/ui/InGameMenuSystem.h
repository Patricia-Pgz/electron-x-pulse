#pragma once
#include "engine/Game.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::game::ui
{
    class InGameMenuSystem final : public engine::ui::IUISubsystem
    {
    public:
        explicit InGameMenuSystem(ImGuiIO* imguiIO, engine::Game& game) : IUISubsystem(imguiIO, game)
        {
        };
        void update() override;

        void setActive(const bool setActive) override
        {
            is_active = setActive;
            show_ui = false;
        }

        void showUI(const bool showUI)
        {
            show_ui = showUI;
        }

    private:
        void DrawInGameUI(const ImGuiViewport* viewport, ImFont* font);
        bool escape_pressed_ = false;
        bool show_ui = false;
        float volume_ = 1.0f;
        static constexpr ImGuiWindowFlags flags_ =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;
    };
}
