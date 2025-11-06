/**
* @file UIEvents.h
 * @brief Provides general events for important user interfaces.
 * For game events, @see GameEvents.h
 */
#pragma once
#include <imgui.h>

#include "engine/levelloading/Objects.h"

namespace gl3::engine::ui
{
    /**
     * Signal to finish grouping and possibly start new group
     */
    struct FinalizeGroup
    {

    };

    /**
 * Signal to cancel grouping
 */
    struct CancelGrouping{};

    /**
 * Button to change to edit mode was pressed.
 */
    struct EditModeButtonPress
    {
        bool edit = true;
    };

    /**
     * Tile was selected in editor. This signals to place it in game.
     */
    struct EditorTileSelectedEvent
    {
        GameObject object;
        bool group = false;
    };

    /**
     * Signals that a tile that was about to be grouped was deleted.
     */
    struct EditorGroupTileDeleted
    {
        std::vector<ImVec2> selectedCells;
    };


    /**
* Call LevelSelect event, whenever a different level is selected. Listen to this event and apply changes accordingly.
*/
    struct LevelSelectEvent
    {
        int newLevelIndex;
    };

    struct StartLevelEvent
    {
        bool startLevel;
    };

    struct PauseLevelEvent
    {
        bool pauseLevel;
    };

    /** Signal that you want to restart the current level
     *
     */
    struct RestartLevelEvent
    {
        ///startLevel determines if the level should directly start after reload
        bool startLevel;
    };

    /**
     * Call this event, when unloading a level
     */
    struct LevelUnload
    {
    };


    /**
   * Call this event to Change the global audio volume in @ref AudioSystem
   * Game already listens to it and applies the change.
   */
    struct VolumeChangeEvent
    {
        float newVolume;
    };
}
