#include "EditorState.h"

#include "Game.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::game::state
{
    void EditorState::onMenuUI(const events::ShowGameMenu& event)
    {
        isShowingMenu = event.showMenu;
        editor_ui_system->setActive(!isShowingMenu);
    }

    void EditorState::onFinishUI(const events::ShowFinishScreen& event)
    {
        isShowingFinishScreen = event.showScreen;
        editor_ui_system->setActive(!isShowingFinishScreen);
    }

    void EditorState::update(const float deltaTime)
    {
        if (isShowingMenu || isShowingFinishScreen) return;
        //start or reset the level on enter press
        if (glfwGetKey(game.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            if (!enter_pressed)
            {
                enter_pressed = true;
                //if level was paused, start it and vice versa
                engine::ecs::EventDispatcher::dispatcher.trigger(engine::ui::RestartLevelEvent{game.isPaused()});
                game.getContext().setCameraPosAndCenter({0.0f, 0.0f, 1.0f}, {0.f, 0.f, 0.f});
            }
        }
        else if (glfwGetKey(game.getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        {
            enter_pressed = false;
        }
    }
}
