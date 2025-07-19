#pragma once
#include <imgui.h>
#include <imgui_internal.h> //need this in every sub system
#include "engine/Game.h"

namespace gl3::engine::ui
{
    /**
     * @class IUISubsystem
     * @brief Interface for all in-game UI systems.
     *
     * Derive from this interface to implement custom UI panels, overlays,
     * editors, or any ImGui-based UI elements in the game. Use it for minimal UIs.
     *
     * @note Every custom UI must be registered with the main UI system via
     * @ref gl3::engine::ui::UISystem::registerSubsystem in your game's
     * @ref gl3::engine::Game::registerUiSystems implementation.
     */
    class IUISubsystem
    {
    public:
        /**
         * @brief Constructor for a UI subsystem.
         * @param imguiIO Pointer to the ImGuiIO context.
         * @param game Reference to the game instance.
         */
        explicit IUISubsystem(ImGuiIO* imguiIO, Game& game): imgui_io(imguiIO), game(game)
        {
        };

        /**
         * @brief Called once per frame to update the UI.
         *
         * Override this to define your UI rendering logic.
         *
         * @pre ImGui must already be initialized.
         * For example, call @ref gl3::engine::ui::UISystem::renderUI in your
         * game loop's UI pass (e.g. @ref gl3::engine::Game::updateUI).
         * @pre An ImGui frame must already be started.
         */
        virtual void update(float deltaTime)
        {
        };

        /**
         * @brief Virtual destructor.
         */
        virtual ~IUISubsystem() = default;

        /**
         * @brief Enable or disable this UI subsystem.
         * @param setActive True to activate, false to deactivate.
         */
        virtual void setActive(const bool setActive)
        {
            is_active = setActive;
        }

        /**
         * @brief Check if this UI subsystem is active.
         * @return True if active, false otherwise.
         */
        virtual bool isActive()
        {
            return is_active;
        }

    protected:
        bool is_active = false;
        ImGuiIO* imgui_io = nullptr;
        Game& game;
    };
}
