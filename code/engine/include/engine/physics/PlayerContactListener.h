#pragma once
#include <iostream>
#include "box2d/box2d.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"

namespace gl3::engine::physics
{
    /**
     * @class PlayerContactListener
     * @brief Utility class for handling player-specific physics contacts and sensor checks.
     *
     * Checks player contact with the ground and obstacles using Box2D sensor events
     * and contacts. Triggers events like player death when necessary.
     */
    class PlayerContactListener
    {
    public:
        /**
         * @brief Indicates whether the player is currently grounded.
         *
         * This flag is set when the player's ground sensor detects contact.
         * It can be reset externally after the physics step to handle jumping logic.
         */
        static inline bool playerGrounded = true;

        /**
         * @brief Checks for collisions and sensor events related to the player.
         *
         * This method:
         * - Inspects sensor begin touch events for the player’s ground and right wall sensors.
         * - Sets the `playerGrounded` flag when the ground sensor is triggered.
         * - Triggers a `PlayerDeath` event when the right wall sensor or obstacle is touched.
         * - Iterates over physical contacts for the player body to check for collisions with obstacles.
         *
         * @param registry Reference to the ECS registry.
         * @param player The player entity to check collisions for.
         * @param physicsWorld The Box2D physics world ID.
         */
        static void checkForPlayerCollision(entt::registry& registry, const entt::entity player,
                                            const b2WorldId physicsWorld)
        {
            // Check player sensor events (right side, ground)
            const b2SensorEvents sensorEvents = b2World_GetSensorEvents(physicsWorld);
            const ecs::PhysicsComponent& physics_comp = registry.get<ecs::PhysicsComponent>(player);

            const b2ShapeId playerGroundSensor = physics_comp.groundSensorShape;
            const b2ShapeId playerRightSensor = physics_comp.rightWallSensorShape;

            for (int i = 0; i < sensorEvents.beginCount; ++i)
            {
                const b2SensorBeginTouchEvent& event = sensorEvents.beginEvents[i];
                if (B2_ID_EQUALS(event.sensorShapeId, playerGroundSensor))
                {
                    playerGrounded = true;
                }
                else if (B2_ID_EQUALS(event.sensorShapeId, playerRightSensor))
                {
                    ecs::EventDispatcher::dispatcher.trigger(ecs::PlayerDeath{player});
                    return;
                }
            }

            // Inspect direct body contacts
            const auto body = physics_comp.body;
            int capacity = b2Body_GetContactCapacity(body);
            capacity = b2MinInt(capacity, 4); // Cap to 4 for simplicity
            b2ContactData contactData[4];
            const int count = b2Body_GetContactData(body, contactData, capacity);

            for (int i = 0; i < count; ++i)
            {
                const b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
                const b2BodyId bodyIdB = b2Shape_GetBody(contactData[i].shapeIdB);

                const auto entityA = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(b2Body_GetUserData(bodyIdA)));
                const auto entityB = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(b2Body_GetUserData(bodyIdB)));

                if (!registry.valid(entityA) || !registry.valid(entityB))
                {
                    std::cout << "Collision with invalid entity\n";
                    continue;
                }

                const auto tagA = registry.get<ecs::TagComponent>(entityA).tag;
                const auto tagB = registry.get<ecs::TagComponent>(entityB).tag;

                if (tagA == "obstacle" || tagB == "obstacle")
                {
                    ecs::EventDispatcher::dispatcher.trigger(ecs::PlayerDeath{player});
                }
            }
        }
    };
} // namespace gl3::engine::physics
