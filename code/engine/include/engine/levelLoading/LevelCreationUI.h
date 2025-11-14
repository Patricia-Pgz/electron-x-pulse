#pragma once
#include <engine/userInterface/IUISubSystem.h>

#include "engine/ecs/EventDispatcher.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::engine::levelLoading
{
    /**
     * @class LevelCreationUISystem
     * @brief User Interface system for creating levels for the game.
     *
     * This UI subsystem provides functionality to display available backgrounds, soundtracks and other level info, that can be set
     * via an ImGui-based interface.
     */
    class LevelCreationUISystem final : public ui::IUISubsystem
    {
    public:
        /**
         * @brief Constructs the LevelCreationUISystem with the provided ImGui IO and game reference.
         *
         * @param imguiIO Pointer to ImGui input/output context.
         * @param game Reference to the main game engine instance.
         */
        explicit LevelCreationUISystem(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game)
        {
            is_active = false;
            ecs::EventDispatcher::dispatcher.sink<ui::CreateLevel>().connect<&LevelCreationUISystem::onCreateUI>(this);
        };

        ~LevelCreationUISystem() override
        {
            ecs::EventDispatcher::dispatcher.sink<ui::CreateLevel>().disconnect<&LevelCreationUISystem::onCreateUI>(this);
        }

        void update(float deltaTime) override;

    private:
        void createUI();
        void DrawLevelForm(const ImGuiViewport* viewport, ImFont* font);
        void onCreateUI(const ui::CreateLevel& event);
        void cancelCreation();

        char name_input_buffer[128] = ""; /**< Input buffer for level name text selection. */
        bool use_solid_bg_color = false; /**< Whether to apply custom coloring to the background. */
        bool use_gradient_bg_color = false; /**< Whether to apply custom gradient coloring to the background. */
        glm::vec4 clear_color = {1.0f, 1.0f, 1.0f, 1.0f}; /**< Color used for background. */
        glm::vec4 top_gradient = {1.0f, 1.0f, 1.0f, 1.0f}; /**< Color used for top of background gradient color. */
        glm::vec4 bottom_gradient = {1.0f, 1.0f, 1.0f, 1.0f}; /**< Color used for bottom of background gradient color. */
        bool picker_was_open = false;
        int current_audio_item = 0;
        int current_bg_item = 0;

    };
}
