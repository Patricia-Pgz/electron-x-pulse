#pragma once

#include "engine/levelEditor/EditorUISystem.h"
#include "engine/stateManagement/GameState.h"
#include "engine/userInterface/UISystem.h"
#include "ui/UIEvents.h"

namespace gl3::game::state
{
    /**
     * @class EditorState
     * @brief Represents the editor mode state in the game.
     *
     * Controls activation of the editor UI, camera resets, and responds to UI events
     * like showing the finish screen or the game menu.
     */
    class EditorState final : public engine::state::GameState
    {
    public:
        /**
         * @brief Constructs the EditorState and subscribes to relevant UI events.
         * @param game Reference to the main game instance.
         */
        explicit EditorState(engine::Game& game)
            : GameState(game)
        {
            editor_ui_system = game.getUISystem()->getSubsystem<engine::editor::EditorUISystem>();

            engine::ecs::EventDispatcher::dispatcher
                .sink<events::ShowFinishScreen>()
                .connect<&EditorState::onFinishUI>(this);

            engine::ecs::EventDispatcher::dispatcher
                .sink<events::ShowGameMenu>()
                .connect<&EditorState::onMenuUI>(this);
        }

        /**
         * @brief Disconnects from subscribed UI events.
         */
        ~EditorState() override
        {
            engine::ecs::EventDispatcher::dispatcher
                .sink<events::ShowFinishScreen>()
                .disconnect<&EditorState::onFinishUI>(this);

            engine::ecs::EventDispatcher::dispatcher
                .sink<events::ShowGameMenu>()
                .disconnect<&EditorState::onMenuUI>(this);
        }

        /**
         * @brief Called when entering the editor state.
         * Enables the editor UI system.
         */
        void onEnter() override
        {
            editor_ui_system->setActive(true);
        }

        /**
         * @brief Called when exiting the editor state.
         * Disables the editor UI system and resets the camera.
         */
        void onExit() override
        {
            enter_pressed = false;
            editor_ui_system->setActive(false);

            // Reset camera to default position and center.
            game.getContext().setCameraPosAndCenter(
                {0.0f, 0.0f, 1.0f},
                {0.f, 0.f, 0.f}
            );
        }

        /**
         * @brief Handles the editor state when the game menu gets shown/hidden
         * @param event ShowGameMenu event.
         */
        void onMenuUI(const events::ShowGameMenu& event);

        /**
         * @brief Handles the editor state when finish screen gets shown/hidden
         * @param event ShowFinishScreen event.
         */
        void onFinishUI(const events::ShowFinishScreen& event);

        /**
         * @brief Called every frame while this state is active.
         * @param deltaTime Elapsed time since last update.
         */
        void update(float deltaTime) override;

    private:
        engine::editor::EditorUISystem* editor_ui_system; ///< Reference to the editor UI system.
        bool enter_pressed = false; ///< Tracks if the enter key has been pressed.
        bool isShowingMenu = false; ///< Is the game menu currently being shown?
        bool isShowingFinishScreen = false; ///< Is the finish screen currently being shown?
    };
} // namespace gl3::game::state
