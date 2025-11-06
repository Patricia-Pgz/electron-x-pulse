#pragma once
#include "engine/ecs/EventDispatcher.h"
#include "engine/Game.h"
#include "engine/levelLoading/Objects.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::engine::editor
{
 /**
* @class EditorSystem
* @brief Manages editor logic and responds to editor UI events.
*
* The EditorSystem handles interactions such as tile selection and group generation
* by listening to editor-related UI events. It maintains collections of child entities and objects to group on event.
*/
 class EditorSystem
 {
 public:
  /**
   * @brief Constructs an EditorSystem instance.
   * @param game Reference to the main game engine.
   *
   * Connects event handlers for tile selection and group generation events.
   */
  explicit EditorSystem(Game& game);

  /**
   * @brief Destructor.
   *
   * Disconnects the event handlers to avoid dangling connections.
   */
  ~EditorSystem();

 private:
  Game& game; /**< Reference to the game instance. */
  entt::entity current_parent_entity = entt::null; ///< Parent entity for physics grouping.
  b2BodyId current_parent_body_id = b2_nullBodyId; ///< Parent BodyID for physics grouping.
  GameObjectGroup current_group;

  /**
   * Creates the actual entity from the tile, selected in editor, and adds it to the current level.
   * Saves children to group for later.
   * @param event The EditorTileSelectedEvent sent by EditorUISystem, once a tile was selected to be placed
   */
  void onTileSelected(ui::EditorTileSelectedEvent& event);

  /**
   * Erase the deleted tile(s) from child arrays for grouping.
   * @param event A tile was deleted during grouping.
   */
  void onFinalizeGroup(const ui::FinalizeGroup& event);

  void onGroupCanceled(const ui::CancelGrouping& event);

 };
} // gl3
