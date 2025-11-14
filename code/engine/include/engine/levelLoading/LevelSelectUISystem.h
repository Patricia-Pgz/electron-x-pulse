#pragma once
#include <engine/userInterface/IUISubSystem.h>

#include "engine/userInterface/UIConstants.h"

namespace gl3::engine::levelLoading
{
    /**
     * Helper function for uniformly styling windows.
     * @note call @ref popWindowStyle() to remove styling again.
     * @param windowSize The current window size.
     */
    inline void styleWindow(const ImVec2 windowSize)
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, UINeonColors::Cyan);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 4.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {windowSize.x * 0.08f, windowSize.y * 0.145f});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));
    }

    /**
     * @brief Removes the ImGui styles, that were added by @ref styleWindow()
     */
    inline void popWindowStyle()
    {
        ImGui::PopStyleColor(8);
        ImGui::PopStyleVar(5);
    }

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
        bool isDeletingLvl = false; ///< User activated level deletion mode.
    };
}
