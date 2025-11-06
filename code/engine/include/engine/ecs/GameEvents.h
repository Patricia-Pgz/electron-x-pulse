/**
* @file GameEvents.h
 * @brief Provides general events for the game.
 * For ui events, @see UIEvents.h
 */
#pragma once
#include "engine/Game.h"

namespace gl3::engine::ecs
{
    /**
 * Signal that instances to render have changed for back to front sorting.
 */
    struct RenderComponentContainerChange{};

    /**
    * Use GameStateChange event to track your current game state, and react to it changing.
    *@property newLevelIndex is used for GameState::Level
     */
    struct GameStateChange
    {
        GameState newGameState = GameState::LevelSelect;
        int newLevelIndex = -1;
    };

    /**
    * Call this event, when the level length is computed based on speed and seconds per beat. Pass in the index of the final beat.
    *
    */
    struct LevelLengthComputed
    {
        float levelLength = 0.f;
        float levelSpeed = 0.f;
        float finalBeatIndex = 0.f;
    };

    /**
    * Call this event, when the level is loaded and ready to play.
    *
     */
    struct LevelStartEvent
    {
        entt::entity player;
    };

    /**
    * Call this event, when the level is in Edit Mode and starts to play.
    *
    */
    struct EditorPlayModeChange
    {
        bool isPlayMode = false;
    };

    /**
    *Call GameExit event, when you want to close the game. E.g. on clicking an exit button, etc.
    */
    struct GameExit
    {
        bool exitGame;
    };

    /**
    * Call this when player jumps.
    */
    struct PlayerJump
    {
        bool grounded = true;
    };

    /**
    * PlayerDeath event gets called, whenever the player collides with an obstacle or hits an object from the left.
    */
    struct PlayerDeath
    {
        entt::entity player;
    };

    /**
     * Signal that a collider was hit, that can change gravity
     */
    struct GravityChange
    {
        b2ShapeId gravityChangerID = b2_nullShapeId;
    };
}
