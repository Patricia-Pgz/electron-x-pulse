#pragma once
#include "IUISubSystem.h"
#include "engine/Game.h"

namespace gl3::engine::inGameUI
{
    class InGameMenuSystem : public ui::IUISubsystem
    {
    public:
        explicit InGameMenuSystem(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game)
        {
        };

        void update() override;

    private:
        void DrawInGameUI(const ImGuiViewport* viewport, ImFont* font);
        bool escape_pressed_ = false;
        bool show_menu_ = false;
        float volume_ = 1.0f;
        static constexpr ImGuiWindowFlags flags_ =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
    };
}
