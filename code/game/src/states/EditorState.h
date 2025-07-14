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
            editor_ui_system = game.getUISystem()->getSubsystem<engine::editor::EditorUISystem>();
        };

        void onEnter() override
        {
            editor_ui_system->setActive(true);
        }

        void onExit() override
        {
            enter_pressed = false;
            play_test = false;
            editor_ui_system->setActive(false);
            game.getContext().setCameraPosAndCenter({0.0f, 0.0f, 1.0f}, {0.f, 0.f, 0.f});
        }

        void update(float deltaTime) override;

    private:
        engine::editor::EditorUISystem* editor_ui_system;
        bool enter_pressed = false;
        bool play_test = false;
    };
}
