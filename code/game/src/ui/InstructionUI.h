#pragma once
#include "engine/Game.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/userInterface/IUISubSystem.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::game::ui
{
    class InstructionUI final : public engine::ui::IUISubsystem
    {
    public:
        explicit InstructionUI(ImGuiIO* imguiIO, engine::Game& game) : IUISubsystem(imguiIO, game)
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::RestartLevelEvent>().connect<&
                InstructionUI::onRestartLevel>(this);
        }
        ~InstructionUI() override {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::RestartLevelEvent>().disconnect<&
                InstructionUI::onRestartLevel>(this);
        }

        void update() override;
        void setActive(bool setActive) override;

        void pauseTimer(const bool pause)
        {
            pause_timer_ = pause;
        }

        void resetTimer();

        void setEditMode(const bool isEditing)
        {
            edit_mode_ = isEditing;
        }

    private:
        void drawHints(const ImGuiViewport* viewport, ImFont* font);
        void onRestartLevel();
        bool edit_mode_ = false;
        bool pause_timer_ = false;
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
