/**
* @file UIEvents.h
 * @brief Provides general events for important user interfaces.
 * For game events, @see GameEvents.h
 */
#pragma once

namespace gl3::engine::ui
{
    /**
* Call LevelSelect event, whenever a different level is selected. Listen to this event and apply changes accordingly.
*/
    struct LevelSelectEvent
    {
        int newLevelIndex;
    };

    struct PauseLevelEvent
    {
        bool pauseLevel;
    };

    struct RestartLevelEvent
    {
        bool restartLevel;
    };

    struct StartLevelEvent
    {
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
