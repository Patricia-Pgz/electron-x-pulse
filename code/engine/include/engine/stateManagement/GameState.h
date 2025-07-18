#pragma once

namespace gl3::engine::state
{
 /**
  * @class GameState
  * @brief Abstract base class for representing a game state. Game states can be handled, using @ref StateManagementSystem
  *
  * States control behavior during different phases of the game (e.g. Menu, Editor, LevelSelect).
  * States can also define update, entry and exit logic.
  */
 class GameState
 {
 public:
  /**
   * @brief Construct a new GameState.
   * @param game Reference to the main Game instance.
   */
  explicit GameState(Game& game) : game(game)
  {
  }

  /**
   * @brief Virtual destructor.
   */
  virtual ~GameState() = default;

  /**
   * @brief Update logic for the state.
   * @param deltaTime Time elapsed since last frame.
   */
  virtual void update(float deltaTime) = 0;

  /**
   * @brief Hook called (e.g. by @ref StateManagementSystem) when the state becomes active.
   *
   * Override to initialize or reset resources specific to this state.
   */
  virtual void onEnter()
  {
  }

  /**
   * @brief Hook called (e.g. by @ref StateManagementSystem) when the state is exited.
   *
   * Override to clean up or save state-specific data.
   */
  virtual void onExit()
  {
  }

 protected:
  /// Reference to the main Game instance.
  Game& game;
 };
}
