#pragma once
#include <iostream>
#include "box2d/box2d.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/PlayerEvents.h"


namespace gl3::engine::physics {

    class PlayerContactListener {
    public:
        static void checkForPlayerCollision(entt::registry& registry, const entt::entity player) {

            ecs::PhysicsComponent& physics_comp = registry.get<ecs::PhysicsComponent>(player);
            auto body = physics_comp.body;
            int capacity = b2Body_GetContactCapacity(body);
            capacity = b2MinInt(capacity, 4); // max of 4 contacts for simplicity
            b2ContactData contactData[4];
            int count = b2Body_GetContactData(body, contactData, capacity);

            for (int i = 0; i < count; ++i)
            {
                b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
                b2BodyId bodyIdB = b2Shape_GetBody(contactData[i].shapeIdB);
                float sign = (B2_ID_EQUALS(bodyIdA, body)) ? -1.0f : 1.0f;
                const auto entityA = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(b2Body_GetUserData(bodyIdA)));
                const auto entityB = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(b2Body_GetUserData(bodyIdB)));
                const auto tagA = registry.get<ecs::TagComponent>(entityA).tag;
                const auto tagB = registry.get<ecs::TagComponent>(entityB).tag;

                if (!registry.valid(entityA) || !registry.valid(entityB))
                {
                    std::cout << "Collision with null";
                    continue;
                }
                if (tagA == "obstacle" || tagB == "obstacle" || contactData[i].manifold.normal.x > 0.9f)
                {
                    // collision with obstacle or player is touching an object from the left -> die
                    ecs::EventDispatcher::dispatcher.trigger(ecs::PlayerDeath{ player });
                }
                else if (sign * contactData[i].manifold.normal.y > 0.9f)
                {
                    // If the normal of the contact is nearly vertical and pointing upwards, player can jump
                    ecs::EventDispatcher::dispatcher.trigger(ecs::PlayerGrounded{ player });
                }
            }
        }
    };
}
