#pragma once
#include "engine/Game.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::game::ui
{
    class InstructionUI final : public engine::ui::IUISubsystem
    {
    public:
        explicit InstructionUI(ImGuiIO* imguiIO, engine::Game& game) : IUISubsystem(imguiIO, game)
        {
        }

        void update() override;

    private:
        void DrawHints(const ImGuiViewport* viewport, ImFont* font) const;
        bool show_hints_ = true;
        static constexpr ImGuiWindowFlags flags_ =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
    };
} // gl3

