#pragma once
#include <iostream>
#include "box2d/box2d.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/ExampleEvents.h"


namespace gl3 {

    class PlayerContactListener {
    public:
        static void checkForPlayerCollision(entt::registry& registry, const entt::entity player) {

            engine::ecs::PhysicsComponent& physics_comp = registry.get<engine::ecs::PhysicsComponent>(player);
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
                const auto tagA = registry.get<engine::ecs::TagComponent>(entityA).tag;
                const auto tagB = registry.get<engine::ecs::TagComponent>(entityB).tag;
                if (!registry.valid(entityA) || !registry.valid(entityB))
                {
                    std::cout << "Collision with null";
                    continue;
                }
                if (tagA == "obstacle" || tagB == "obstacle")
                {
                    std::cout << "Hit Obstacle, reset to start";
                    //onPlayerDeath.invoke();
                }
                else if (sign * contactData[i].manifold.normal.y > 0.9f)
                {
                    // If the normal of the contact is nearly vertical and pointing upwards, we can jump
                    //canJump = true;
                    engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayerGrounded{ player });

                }
                else if (contactData[i].manifold.normal.x > 0.9f)
                {
                    // Almost horizontal collision to the left
                    // Player is touching from the left
                    std::cout << "Destroy Player, reset to start";
                    //onPlayerDeath.invoke();
                }
            }









            /*
            b2ContactEvents contactEvents = b2World_GetContactEvents(world);
            for (int i = 0; i < contactEvents.beginCount; ++i) {
                const b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
                const auto bodyUserDataA = getUserDataOfShape(beginEvent->shapeIdA);
                const auto bodyUserDataB = getUserDataOfShape(beginEvent->shapeIdB);

                if(bodyUserDataA && bodyUserDataB)
                {
                    const auto entityA = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(bodyUserDataA));
                    const auto entityB = static_cast<entt::entity>(reinterpret_cast<uintptr_t>(bodyUserDataB));
                    if (!registry.valid(entityA) || !registry.valid(entityB)) continue;
                    auto tagA = registry.get<engine::ecs::TagComponent>(entityA).tag;
                    auto tagB = registry.get<engine::ecs::TagComponent>(entityB).tag;
                    auto sign = tagA == "player"? -1.0f : 1.0f;
                    if(tagA != "player" && tagB != "player") continue;
                    auto player = tagA == "player"? entityA : entityB;
                    std::cout << tagA << std::endl;
                    std::cout << tagB << std::endl;
                    std::cout << "Normal Y: " << beginEvent->manifold.normal.y << std::endl;
                    if (sign * beginEvent->manifold.normal.y > 0.9f)
                    {
                        // If the normal of the contact is nearly vertical and pointing upwards, player can jump
                        engine::ecs::EventManager::dispatcher.trigger(engine::ecs::PlayerGrounded{ player });
                    }
                    else if (beginEvent->manifold.normal.x > 0.9f)
                    {
                        // Almost horizontal collision to the left
                        // Player is touching from the left
                        std::cout << "Destroy Player, reset to start";
                        //engine::ecs::EventManager::dispatcher.trigger(engine::ecs::PlayerDeath{ player });
                        //onPlayerDeath.invoke(); //TODO playerdeathevent
                    }else if(tagA == "obstacle" || tagB == "obstacle"){
                        std::cout << "Hit Obstacle, reset to start"; //TODO put logic in ContactListener!
                        //engine::ecs::EventManager::dispatcher.trigger(engine::ecs::PlayerDeath{ player }); //TODO A oder B is player
                        //onPlayerDeath.invoke(); //TODO event in ContactListener
                    }

                }
            }*/
        }
    };
}
