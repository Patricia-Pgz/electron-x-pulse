/**
* @file UIEvents.h
 * @brief Provides general events for important user interfaces.
 * For game events, @see GameEvents.h
 */
#pragma once

namespace gl3::engine::ui
{

    /**
 * Button to change to edit mode was pressed.
 */
    struct EditeModeButtonPress
    {
        bool edit = true;
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
        /** startLevel determines if the level should directly start after reload
 *
 */
        bool startLevel;
    };


    /**
   * Call this event to Change the volume of "audio_" in @ref Game.h
   * Game already listens to it and applies the change.
   */
    struct VolumeChangeEvent
    {
        float newVolume;
    };
}
