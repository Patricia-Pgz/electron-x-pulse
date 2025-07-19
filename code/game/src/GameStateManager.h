/**
 * @file GameStateManager.h
 * @brief Declares the GameStateManager class for managing high-level game states.
 */

#pragma once

#include "Game.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::game
{
 /**
  * @class GameStateManager
  * @brief Handles the game state transitions and edit mode toggling.
  *
  * Manages switching between states like LevelSelectState, LevelPlayState, EditorState, and responds to UI events.
  */
 class GameStateManager
 {
 public:
  /**
   * @brief Constructs the GameStateManager.
   * @param game Reference to the owning Game instance.
   */
  explicit GameStateManager(Game& game);

  /**
   * @brief Destroys the GameStateManager and unregisters event handlers.
   */
  ~GameStateManager();

  /**
   * @brief Gets the current game state.
   * @return Reference to the current engine::GameState.
   */
  [[nodiscard]] const engine::GameState& getCurrentState() const
  {
   return current_game_state;
  }

  /**
   * @brief Gets the previous game state.
   * @return Reference to the previous engine::GameState.
   */
  [[nodiscard]] const engine::GameState& getPreviousState() const
  {
   return previous_game_state;
  }

 private:
  /**
   * @brief Callback for when the UI is initialized.
   * Initializes classes that need UI Systems to already be initialized.
   */
  void onUiInitialized() const;

  /**
   * @brief Callback for handling edit mode toggle events.
   * @param event EditModeButtonPress event containing the new state.
   */
  void onEditModeChange(const engine::ui::EditModeButtonPress& event);

  /**
   * @brief Callback for handling game state change events.
   * @param newState Event containing the new GameState.
   */
  void onGameStateChange(const engine::ecs::GameStateChange& newState);

  /// Reference to the owning Game instance.
  Game& game;

  /// Handle for the UI initialization event connection.
  engine::events::Event<engine::ui::UISystem>::handle_t onUIInitHandle;

  /// Currently active game state.
  engine::GameState current_game_state = engine::GameState::None;

  /// Previously active game state.
  engine::GameState previous_game_state = engine::GameState::None;

  /// Indicates whether edit mode is active.
  bool is_edit_mode = false;
 };
}
