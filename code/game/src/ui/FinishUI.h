#pragma once
#include "engine/Game.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::game::ui
{
    class FinishUI final : public engine::ui::IUISubsystem
    {
    public:
        explicit FinishUI(ImGuiIO* imguiIO, engine::Game& game) : IUISubsystem(imguiIO, game)
        {
        };

        void update() override;

    private:
        void styleWindow(ImVec2 windowSize);
        void DrawInGameUI(const ImGuiViewport* viewport, ImFont* font);
        bool escape_pressed_ = false;
        static constexpr ImGuiWindowFlags flags_ =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;
    };
}
