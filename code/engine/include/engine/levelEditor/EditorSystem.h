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
  std::vector<entt::entity> grouped_child_entities; /**< Entities grouped together in the editor. */
  std::vector<GameObject> grouped_child_objects; /**< GameObjects grouped together in the editor. */

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
  void onGroupTileDeleted(const ui::EditorGroupTileDeleted& event);


  /**
   * Generates an AABB physics Component for previously grouped objects/entities and adds ParentComponent and GroupComponent. Adds new group to current level.
   * @param event The EditorGenerateGroup sent by EditorUISystem, once the button to generate a group was pressed.
   */
  void onGenerateGroup(ui::EditorGenerateGroup& event);

  /**
   * Grouping was canceled -> clear saved group objects
   * @param event Grouping has been canceled
   */
  void onGroupCanceled(ui::EditorCancelGrouping& event);
 };
} // gl3
