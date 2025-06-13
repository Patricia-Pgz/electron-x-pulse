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
    *Use GameStateChange event to track your current game state, and react to it changing.
    *
     */
    struct GameStateChange
    {
        GameState newGameState = GameState::LevelSelect;
        int newLevelIndex = -1;
    };

    /**
    *Call GameExit event, when you want to close the game. E.g. on clicking an exit button, etc.
    */
    struct GameExit
    {
        bool exitGame;
    };

    /**
    *PlayerGrounded event gets called, whenever the normal of a player contact, that is not an obstacle, is nearly vertical and pointing upwards. Meaning the player lands on top of a walkable object.
    */
    struct PlayerGrounded
    {
        entt::entity player;
    };

    /**
    *PlayerDeath event gets called, whenever the player collides with an obstacle or hits an object from the left.
    */
    struct PlayerDeath
    {
        entt::entity player;
    };

}
