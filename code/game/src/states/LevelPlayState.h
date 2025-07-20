#pragma once
#include "engine/Game.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/levelloading/Objects.h"
#include "engine/stateManagement/GameState.h"
#include "engine/userInterface/UISystem.h"
#include "ui/FinishUI.h"
#include "ui/InGameMenuUI.h"
#include "ui/InstructionUI.h"

namespace gl3::game::state
{
 /**
  * @brief Describes background dimensions for the current level,
  *        computed from window bounds.
  */
 struct LevelBackgroundConfig
 {
  float centerX;
  float windowWidth;

  float groundCenterY;
  float groundHeight;

  float skyCenterY;
  float skyHeight;
 };

 /**
  *@class LevelPlayState
  * @brief Game state responsible for active level play.
  *
  * Handles player death, pausing, restarting, and reacts to window size adjustments.
  */
 class LevelPlayState final : public engine::state::GameState
 {
 public:
  /**
   * @brief Construct a LevelPlayState.
   * @param game Reference to game instance.
   * @param levelIndex Index of the level to play.
   * @param editMode True if in edit mode.
   */
  explicit LevelPlayState(engine::Game& game, const int levelIndex, const bool editMode)
   : GameState(game), edit_mode(editMode), level_index(levelIndex)
  {
   const auto& topLvlUI = game.getUISystem();
   //get all the in-game UIs to handle
   menu_ui = topLvlUI->getSubsystem<ui::InGameMenuUI>();
   instruction_ui = topLvlUI->getSubsystem<ui::InstructionUI>();
   finish_ui = topLvlUI->getSubsystem<ui::FinishUI>();

   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::ecs::PlayerDeath>()
    .connect<&LevelPlayState::onPlayerDeath>(this);

   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::ui::RestartLevelEvent>()
    .connect<&LevelPlayState::onRestartLevel>(this);

   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::ui::PauseLevelEvent>()
    .connect<&LevelPlayState::onPauseEvent>(this);

   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::context::WindowBoundsRecomputeEvent>()
    .connect<&LevelPlayState::onWindowSizeChange>(this);
  }

  /**
   * @brief Disconnects all event handlers.
   */
  ~LevelPlayState() override
  {
   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::ecs::PlayerDeath>()
    .disconnect<&LevelPlayState::onPlayerDeath>(this);

   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::ui::RestartLevelEvent>()
    .disconnect<&LevelPlayState::onRestartLevel>(this);

   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::ui::PauseLevelEvent>()
    .disconnect<&LevelPlayState::onPauseEvent>(this);

   engine::ecs::EventDispatcher::dispatcher
    .sink<engine::context::WindowBoundsRecomputeEvent>()
    .disconnect<&LevelPlayState::onWindowSizeChange>(this);
  }

  /**
   * @brief Called when entering the LevelPlayState
   * Activates UI, loads level assets.
   */
  void onEnter() override
  {
   menu_ui->setActive(true);
   if (level_index == 0) // Tutorial Level
   {
    instruction_ui->setActive(true); // Deactivates itself after timer.
   }
   loadLevel();
  }

  /**
   * @brief Called when leaving the level.
   * Cleans up entities and resets state.
   */
  void onExit() override
  {
   unloadLevel();
  }

  /**
   * @brief Runs per-frame logic for the level.
   * @param deltaTime Time elapsed since last frame.
   */
  void update(float deltaTime) override;

 private:
  /**
   * @brief Load level data and instantiate entities.
   */
  void loadLevel();

  /**
   * @brief Enable or disable entity movement.
   */
  void moveObjects(bool move) const;

  /**
   * @brief Pause or resume the level.
   */
  void pauseOrResumeLevel(bool pause);

  /**
   * @brief Enable/disable related systems.
   */
  void setSystemsActive(bool setActive) const;

  /**
   * @brief Reload the current level.
   */
  void reloadLevel();

  /**
   * @brief Unload current level entities and clean up.
   */
  void unloadLevel();

  /**
   * @brief Delay the level end by a timer.
   */
  void delayLevelEnd(float deltaTime);

  /**
   * @brief Handle what happens on player death (->restart).
   */
  void onPlayerDeath(const engine::ecs::PlayerDeath& event);

  /**
   * @brief Handle window resizing.
   */
  void onWindowSizeChange(const engine::context::WindowBoundsRecomputeEvent& event) const;

  /**
   * @brief Compute background layout sizes.
   */
  [[nodiscard]] LevelBackgroundConfig getBackgroundSizes(const std::vector<float>& windowBounds) const;

  /**
   * @brief Create sky gradient entity.
   */
  void createSkyGradientEntity(const LevelBackgroundConfig& bgConfig,
                               entt::registry& registry, b2WorldId physicsWorld) const;

  /**
   * @brief Create background entities.
   */
  void createBackgroundEntities(const LevelBackgroundConfig& bgConfig,
                                entt::registry& registry, b2WorldId physicsWorld) const;

  /**
   * @brief Instantiate grouped entities.
   */
  void createGroupedEntities(entt::registry& registry, b2WorldId physicsWorld) const;

  /**
   * @brief Instantiate single, non-grouped entities.
   */
  void createSingleEntities(entt::registry& registry, b2WorldId physicsWorld);

  /**
   * @brief High-level method for entity creation.
   */
  void createEntities(const LevelBackgroundConfig& bgConfig, entt::registry& registry, b2WorldId physicsWorld);

  /**
   * @brief Initialize audio configuration for the level.
   */
  void initializeAudio();

  /**
   * @brief Restart the current level. (Also gets called on in-game UI events)
   */
  void onRestartLevel(const engine::ui::RestartLevelEvent& event);

  /**
   * @brief Handle pause events sent from in-game UIs.
   */
  void onPauseEvent(const engine::ui::PauseLevelEvent& event);

  /**
   * @brief Reset level entities.
   */
  void resetEntities() const;

  /**
   * @brief Start the level. @note Is used after resetting everything, not to resume level.
   */
  void startLevel();

  // === State ===
  float level_time = 0.f; ///< Timer for this try of the level
  ui::InGameMenuUI* menu_ui = nullptr;
  ui::FinishUI* finish_ui = nullptr;
  ui::InstructionUI* instruction_ui = nullptr;
  engine::audio::AudioConfig* audio_config = nullptr;

  bool edit_mode = false; ///< Is edit mode active
  bool paused = true; ///< Is the level paused
  bool level_instantiated = false;
  bool timer_active = false; ///< Has the timer to end the level been triggered
  bool transition_triggered = false; ///< Has level end transition already been triggered
  bool reloading_level = false; ///< Is the level already restarting

  float timer = 1.f;
  int level_index = -1;

  Level* current_level = nullptr; ///< Pointer to the current level, owned by LevelManager.
  entt::entity current_player = entt::null;
 };
} // namespace gl3::game::state
