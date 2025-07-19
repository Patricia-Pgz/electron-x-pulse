#pragma once
#include "engine/Game.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::game::ui
{
 /**
  * @class InGameMenuUI
  * @brief Handles the in-game pause menu UI using ImGui.
  *
  * Provides functionality to show/hide the in-game menu,
  * and control volume settings.
  */
 class InGameMenuUI final : public engine::ui::IUISubsystem
 {
 public:
  /**
   * Constructs the InGameMenuUI.
   * @brief Constructs the InGameMenuUI subsystem.
   * @param imguiIO Pointer to ImGui IO structure.
   * @param game Reference to the main game instance.
   */
  explicit InGameMenuUI(ImGuiIO* imguiIO, engine::Game& game) : IUISubsystem(imguiIO, game){}

  /**
   * @brief Updates the in-game menu UI once per frame. (if is_active)
   */
  void update(float deltaTime) override;

 private:
  /**
   * @brief Draws the in-game menu UI window.
   * @param viewport Pointer to the ImGui viewport.
   * @param font Pointer to the font used for rendering.
   */
  void DrawInGameUI(const ImGuiViewport* viewport, ImFont* font);

  /// Stores whether the game was playing before pause.
  bool play_mode_before_pause = false;

  /// Temporarily saves the play mode state.
  bool play_mode_saved = false;

  /// Tracks if the escape key has been pressed.
  bool escape_pressed = false;

  /// Indicates if the UI should be drawn this frame.
  bool show_ui = false;

  /// Volume level controlled through the menu (range 0.0 to 1.0).
  float volume = 1.0f;

  /// Window flags for ImGui window style.
  static constexpr ImGuiWindowFlags flags =
   ImGuiWindowFlags_NoMove |
   ImGuiWindowFlags_NoCollapse |
   ImGuiWindowFlags_NoTitleBar;
 };
}
