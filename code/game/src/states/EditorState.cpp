#include "EditorState.h"

#include "engine/userInterface/UIEvents.h"

namespace gl3::game::state
{
    void EditorState::update(const float deltaTime)
    {
        //start or reset the level on enter press
        if (glfwGetKey(game.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            if (!enter_pressed_)
            {
                enter_pressed_ = true;
                play_test_ = !play_test_;
                editor_ui_system_->setActive(!play_test_);
                engine::ecs::EventDispatcher::dispatcher.trigger(engine::ui::RestartLevelEvent{play_test_});
                game.getContext().setCameraPosAndCenter({0.0f, 0.0f, 1.0f}, {0.f, 0.f, 0.f});
            }
        }
        else if (glfwGetKey(game.getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        {
            enter_pressed_ = false;
        }
    }
}
