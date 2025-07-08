#pragma once
#include <iostream>
#include "box2d/box2d.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"


namespace gl3::engine::physics
{
    class PlayerContactListener
    {
    public:
        static inline bool playerGrounded = true;

        static void checkForPlayerCollision(entt::registry& registry, const entt::entity player,
                                            const b2WorldId physicsWorld)
        {
            //check player sensor events (right, bottom)
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

            const auto body = physics_comp.body;
            int capacity = b2Body_GetContactCapacity(body);
            capacity = b2MinInt(capacity, 4); // max of 4 contacts for simplicity
            b2ContactData contactData[4];
            const int count = b2Body_GetContactData(body, contactData, capacity);

            for (int i = 0; i < count; ++i)
            {
                const b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
                const b2BodyId bodyIdB = b2Shape_GetBody(contactData[i].shapeIdB);
                const auto entityA = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(
                    b2Body_GetUserData(bodyIdA)));
                const auto entityB = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(
                    b2Body_GetUserData(bodyIdB)));
                const auto tagA = registry.get<ecs::TagComponent>(entityA).tag;
                const auto tagB = registry.get<ecs::TagComponent>(entityB).tag;

                if (!registry.valid(entityA) || !registry.valid(entityB))
                {
                    std::cout << "Collision with null";
                    continue;
                }
                if (tagA == "obstacle" || tagB == "obstacle")
                {
                    ecs::EventDispatcher::dispatcher.trigger(ecs::PlayerDeath{player});
                }
            }
        }
    };
}
