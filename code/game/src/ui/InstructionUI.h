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
        void setActive(bool setActive) override;

        void pauseTimer(const bool pause)
        {
            pause_timer = pause;
        }

    private:
        void drawHints(const ImGuiViewport* viewport, ImFont* font);
        bool pause_timer = false;
        float timer_ = 15;
        static constexpr ImGuiWindowFlags flags_ =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
    };
} // gl3
