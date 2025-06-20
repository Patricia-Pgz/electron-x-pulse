#pragma once
#include <imgui.h>
#include <imgui_internal.h> //need this in every sub system
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
        };
        virtual ~IUISubsystem() = default;
        void setActive(const bool setActive)
        {
            is_active = setActive;
        }

    protected:
        bool is_active = false;
        ImGuiIO* imgui_io_ = nullptr;
        Game& game_;
    };
}
