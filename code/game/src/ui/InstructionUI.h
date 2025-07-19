#pragma once
#include "engine/Game.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::game::ui
{
    /**
     * @class InstructionUI
     * @brief Displays instructional hints to the player during gameplay.
     *
     * Manages a timer to auto-hide the instructions and responds to level restart events.
     */
    class InstructionUI final : public engine::ui::IUISubsystem
    {
    public:
        /**
         * @brief Constructs the InstructionUI subsystem.
         * @param imguiIO Pointer to ImGui IO structure.
         * @param game Reference to the main game instance.
         */
        explicit InstructionUI(ImGuiIO* imguiIO, engine::Game& game) : IUISubsystem(imguiIO, game){};

        /**
         * @brief Updates the instruction UI each frame.
         */
        void update(float deltaTime) override;

        /**
         * @brief Sets whether the instruction UI is active (visible).
         * @param setActive True to activate, false to deactivate.
         */
        void setActive(bool setActive) override;

        /**
         * @brief Resets the instruction timer to its initial value.
         */
        void resetTimer();

        /**
         * @brief Sets whether the game is in edit mode, affecting UI behavior.
         * @param isEditing True if edit mode is active.
         */
        void setEditMode(bool isEditing);

    private:
        /**
         * @brief Renders the hint messages in the UI.
         * @param viewport Pointer to ImGui viewport.
         * @param font Pointer to the font used for rendering text.
         * @param deltaTime The game's time since the last frame
         */
        void drawHints(const ImGuiViewport* viewport, ImFont* font, float deltaTime);

        /**
         * @brief Handles the RestartLevelEvent to reset the instruction timer.
         */
        void onRestartLevel();

        /// Indicates if the UI is in edit mode.
        bool edit_mode = false;

        /// Timer controlling how long hints are displayed (seconds).
        float timer = 15;

        /// ImGui window flags for styling the instruction window.
        static constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus;
    };
} // namespace gl3::game::ui
