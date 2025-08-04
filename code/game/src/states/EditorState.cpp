#include "EditorState.h"
#include "Game.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::game::state
{
    void EditorState::onMenuUI(const events::ShowGameMenu& event)
    {
        //deactivate editor UI when other UI is activated & activate again, when it's deactivated
        isShowingMenu = event.showMenu;
        editor_ui_system->setActive(!isShowingMenu);
    }

    void EditorState::onFinishUI(const events::ShowFinishScreen& event)
    {
        isShowingFinishScreen = event.showScreen;
        //deactivate editor UI when other UI is activated & activate again, when it's deactivated
        editor_ui_system->setActive(!isShowingFinishScreen);
    }

    void EditorState::update(const float deltaTime)
    {
        // Skip input handling when menu or finish screen is active
        if (isShowingMenu || isShowingFinishScreen) return;

        // Check if ENTER key is pressed (with debounce to avoid multiple triggers)
        if (glfwGetKey(game.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            if (!enter_pressed)
            {
                enter_pressed = true;
                // Signalize to restart level
                engine::ecs::EventDispatcher::dispatcher.trigger(engine::ui::RestartLevelEvent{game.isPaused()});

                // Reset camera position and center (because of editor scrolling)
                game.getContext().setCameraPosAndCenter({0.0f, 0.0f, 1.0f}, {0.f, 0.f, 0.f});
            }
        }
        else
        {
            // Reset debounce flag when ENTER is released
            enter_pressed = false;
        }
    }
}
