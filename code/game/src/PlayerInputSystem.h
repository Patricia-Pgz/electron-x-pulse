#pragma once
#include "box2d/box2d.h"
#include "engine/Game.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/ecs/System.h"

namespace gl3::game::input
{
    /**
     *@class PlayerInputSystem
     * @brief Handles player input logic and responds to relevant game events.
     */
    class PlayerInputSystem final : public engine::ecs::System
    {
    public:
        /**
         * @brief Constructs the PlayerInputSystem and subscribes to level length events.
         * @param game Reference to the main engine Game instance.
         */
        explicit PlayerInputSystem(engine::Game& game) : System(game)
        {
            engine::ecs::EventDispatcher::dispatcher
                .sink<engine::ecs::LevelLengthComputed>()
                .connect<&PlayerInputSystem::onLvlLengthCompute>(this);
            engine::ecs::EventDispatcher::dispatcher
                .sink<engine::ecs::JumpMechanicCollider>()
                .connect<&PlayerInputSystem::onJumpMechanicChange>(this);
        }

        /**
         * @brief Unsubscribes from events and cleans up.
         */
        ~PlayerInputSystem() override
        {
            engine::ecs::EventDispatcher::dispatcher
                .sink<engine::ecs::LevelLengthComputed>()
                .disconnect<&PlayerInputSystem::onLvlLengthCompute>(this);
            engine::ecs::EventDispatcher::dispatcher
    .sink<engine::ecs::JumpMechanicCollider>()
    .disconnect<&PlayerInputSystem::onJumpMechanicChange>(this);
        }

        /**
         * @brief Called each frame to handle input logic.
         */
        void update();

    private:
        /**
         * @brief Handles adjustments when the level length is computed.
         * @param event Contains the computed level length data.
         */
        void onLvlLengthCompute(const engine::ecs::LevelLengthComputed& event);

        /**
         * Changes jump mechanics when a collider triggered the corresponding event (@see PlayerContactListener)
         * @param event a collider to change jump mechanics was hit
         */
        void onJumpMechanicChange(engine::ecs::JumpMechanicCollider& event);

        /**
         * @brief Applies an upward impulse to the player's body to simulate a jump.
         * @param body The Box2D body ID for the player.
         */
        void applyJumpImpulse(b2BodyId body) const;

        float curr_lvl_speed = 1.f; ///< Current level speed.
        bool enter_pressed = false; ///< Tracks if the enter key is pressed.
        bool can_jump = true; ///< Determines if the player can jump.
        float desired_jump_height = 2.f; ///< Desired jump height in units.
        float landing_beats_ahead = 2.f; ///< Anticipation factor for landing in beats.
        entt::entity player = entt::null; ///< Player entity reference.
        float rotation_speed = -270.f; ///< Rotation speed for visual player spin.
        bool change_jump_mechanics = false; ///< True if event to change jump mechanics was triggered.
        float y_gravity_multiplier = 1.f; ///< Controls y gravity.
    };
} // gl3
