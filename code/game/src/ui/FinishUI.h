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
        }

        void update(float deltaTime) override;

    private:
        ///sets the ImGui Style consistently with UINeonColors
        static void styleWindow(ImVec2 windowSize);
        ///Draw the winning screen
        static void DrawFinishScreen(const ImGuiViewport* viewport, ImFont* heading, ImFont* font);
        ///ImGui flags for window
        static constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBackground;
    };
}
