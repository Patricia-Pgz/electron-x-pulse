/**
* @file Game.h
 * @brief Specialized Game class for ElectronXPulse game project.
 */
#pragma once
#include "engine/Game.h"
#include "PlayerInputSystem.h"

namespace gl3::game
{
 class GameStateManager;
 /**
  * @class Game
  * @brief Main game class derived from the engine::Game.
  *
  * Extends the base game loop with project-specific systems like player input and game state management.
  */
 class Game final : public engine::Game
 {
 public:
  /**
   * @brief Constructs the Game instance.
   *
   * @param width Window width in pixels.
   * @param height Window height in pixels.
   * @param title Window title string.
   * @param camPos Initial camera position.
   * @param camZoom Initial camera zoom level.
   */
  Game(int width, int height, const std::string& title, const glm::vec3& camPos, float camZoom);

  /**
   * @brief Retrieves the player input system.
   *
   * @return Pointer to the PlayerInputSystem instance.
   */
  [[nodiscard]] input::PlayerInputSystem* getPlayerInputSystem() const { return player_input_system; }

 private:
  /**
   * @brief Frame update function. Overrides base class update. Update your custom systems here
   * (e.g. calls PlayerInputSystem update each frame)
   * @param window Pointer to the GLFW window.
   */
  void update(GLFWwindow* window) override;

  /**
   * @brief Pre-register UI systems used in the game.
   * Registers engine::levelLoading::LevelSelectUISystem, engine::editor::EditorUISystem, ui::InGameMenuUI, ui::InstructionUI, ui::FinishUI, engine::editor::EditorUISystem
   * Called during startup to add custom ImGui or other UI logic.
   */
  void registerUiSystems() override;

  /// Manages the high-level game state (menus, levels, etc.)
  GameStateManager* game_state_manager = nullptr;

  /// Handles player input.
  input::PlayerInputSystem* player_input_system = nullptr;
 };
}
