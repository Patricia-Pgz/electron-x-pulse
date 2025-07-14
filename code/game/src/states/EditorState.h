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
            editor_ui_system_ = game.getUISystem()->getSubsystem<engine::editor::EditorUISystem>();
        };

        ~EditorState() override = default;

        void onEnter() override
        {
            editor_ui_system_->setActive(true);
        }

        void onExit() override
        {
            enter_pressed_ = false;
            play_test_ = false;
            editor_ui_system_->setActive(false);
            game.getContext().setCameraPosAndCenter({0.0f, 0.0f, 1.0f}, {0.f, 0.f, 0.f});
        }

        void update(float deltaTime) override;

    private:
        engine::editor::EditorUISystem* editor_ui_system_;
        bool enter_pressed_ = false;
        bool play_test_ = false;
    };
}
