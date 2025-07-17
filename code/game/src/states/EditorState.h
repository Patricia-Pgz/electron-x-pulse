#pragma once
#include "engine/levelEditor/EditorUISystem.h"
#include "engine/stateManagement/GameState.h"
#include "engine/userInterface/UISystem.h"
#include "ui/UIEvents.h"


namespace gl3::game::state
{
    class EditorState final : public engine::state::GameState
    {
    public:
        explicit EditorState(engine::Game& game)
            : GameState(game)
        {
            editor_ui_system = game.getUISystem()->getSubsystem<engine::editor::EditorUISystem>();
            engine::ecs::EventDispatcher::dispatcher.sink<events::ShowFinishScreen>().connect<&
                EditorState::onFinishUI>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<events::ShowGameMenu>().connect<&
                EditorState::onMenuUI>(this);
        };

        ~EditorState() override
        {
            engine::ecs::EventDispatcher::dispatcher.sink<events::ShowFinishScreen>().disconnect<&
                EditorState::onFinishUI>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<events::ShowGameMenu>().disconnect<&
                EditorState::onMenuUI>(this);
        }

        void onEnter() override
        {
            editor_ui_system->setActive(true);
        }

        void onExit() override
        {
            enter_pressed = false;
            editor_ui_system->setActive(false);
            game.getContext().setCameraPosAndCenter({0.0f, 0.0f, 1.0f}, {0.f, 0.f, 0.f});
        }

        void onMenuUI(const events::ShowGameMenu& event);
        void onFinishUI(const events::ShowFinishScreen& event);
        void update(float deltaTime) override;

    private:
        engine::editor::EditorUISystem* editor_ui_system;
        bool enter_pressed = false;
        bool isShowingMenu = false;
        bool isShowingFinishScreen = false;
    };
}
