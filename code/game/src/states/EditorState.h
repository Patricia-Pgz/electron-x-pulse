#pragma once
#include "engine/Game.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "engine/stateManagement/GameState.h"
#include "engine/userInterface/UISystem.h"

namespace gl3::game::state
{
    class EditorState final : public engine::state::GameState
    {
    public:
        explicit EditorState(engine::Game& game)
            : GameState(game)
        {
            editor_ui_system_ = game_.getUISystem()->getSubsystem<engine::editor::EditorUISystem>();
        };
        ~EditorState() override = default;

        void onEnter() override
        {
            editor_ui_system_->setActive(true);
        }

        void onExit() override
        {
            editor_ui_system_->setActive(false);
        }

        void update(const float deltaTime) override
        {
        }

    private:
        engine::editor::EditorUISystem* editor_ui_system_;
    };
}
