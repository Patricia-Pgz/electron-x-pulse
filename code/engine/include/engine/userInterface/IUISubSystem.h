#pragma once
#include <imgui.h>
#include <imgui_internal.h> //need this in every sub system
#include "engine/Game.h"

namespace gl3::engine::ui
{
    /**
     * IUISubsystems is an interface to create actual game user interfaces from.
     * @note Every custom UI needs to be registered to the ui system via @ref gl3::engine::ui::UISystem::registerSubsystem() in the game's @ref Game::registerUiSystems()
     */
    class IUISubsystem
    {
    public:
        explicit IUISubsystem(ImGuiIO* imguiIO, Game& game): imgui_io_(imguiIO), game_(game)
        {
        };

        /**
 * update is called each frame.
 * @pre ImGui needs to be set up already. E.g. by calling @ref gl3::engine::ui::UISystem::renderUI on a UISystem instance each game UI-Update frame @ref gl3::engine::Game::updateUI() .
* @pre ImGui Frame needs to be running already.
*/
        virtual void update()
        {
        };
        virtual ~IUISubsystem() = default;

        virtual void setActive(const bool setActive)
        {
            is_active = setActive;
        }

        virtual bool isActive()
        {
            return is_active;
        }

    protected:
        bool is_active = false;
        ImGuiIO* imgui_io_ = nullptr;
        Game& game_;
    };
}
