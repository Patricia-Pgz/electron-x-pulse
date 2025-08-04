#pragma once
#include <engine/userInterface/IUISubSystem.h>

namespace gl3::engine::levelLoading
{
    /**
     * @class LevelSelectUISystem
     * @brief User Interface system for selecting levels within the game.
     *
     * This UI subsystem provides functionality to display available levels and allows
     * users to select them through an ImGui-based interface.
     */
    class LevelSelectUISystem final : public ui::IUISubsystem
    {
    public:
        /**
         * @brief Constructs the LevelSelectUISystem with the provided ImGui IO and game reference.
         *
         * @param imguiIO Pointer to ImGui input/output context.
         * @param game Reference to the main game engine instance.
         */
        explicit LevelSelectUISystem(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game)
        {
        };

        /**
         * @brief Updates the UI elements for the level selection screen, if is_active.
         *
         * Called every frame to draw and handle interactions with the level selection interface.
         */
        void update(float deltaTime) override;

    private:
        /**
         * @brief Draws buttons representing available levels.
         *
         * This static function enumerates levels and creates clickable UI buttons for each.
         */
        static void DrawLevelButtons();

        /**
         * @brief Draws the complete level selection window.
         *
         * @param viewport Pointer to the ImGui viewport where the UI is rendered.
         * @param font Pointer to the font used for rendering text in the UI.
         */
        void DrawLevelSelect(const ImGuiViewport* viewport, ImFont* font);

        /**
         * @brief Initializes and populates the level selection UI components.
         */
        void createLevelSelection();

        bool editModeActive = false; ///< Tracks whether the UI is in edit mode, adjusting visual features.
    };
}
