#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include "FontManager.h"
#include "engine/Game.h"

namespace gl3::engine::ui
{
    class IUISubsystem
    {
    public:
        explicit IUISubsystem(ImGuiIO* imguiIO, Game& game): imgui_io_(imguiIO), game_(game)
        {
        };
        virtual void update()
        {
            if (!ImGui::GetCurrentContext()) return;
        };
        virtual ~IUISubsystem() = default;

    protected:
        ImGuiIO* imgui_io_ = nullptr;
        Game& game_;
    };
}
