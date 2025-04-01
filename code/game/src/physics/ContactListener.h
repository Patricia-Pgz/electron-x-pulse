#pragma once
#include <iostream>
#include "box2d/box2d.h"
#include "engine/ecs/EventManager.h"
#include "engine/ecs/ExampleEvents.h"


namespace gl3 {

    class ContactListener {
    public:
        static void checkForPlayerCollision(const b2WorldId world, entt::registry& registry) {
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
                    std::cout << tagA << std::endl;
                    std::cout << tagB << std::endl;
                    if (sign * beginEvent->manifold.normal.y > 0.9f) //TODO brauch ich die hier beide??
                    {

                        //TODO playerisgrounded event
                        // If the normal of the contact is nearly vertical and pointing upwards, we can jump
                        //canJump = true;
                    }
                    else if (beginEvent->manifold.normal.x > 0.9f)
                    {
                        // Almost horizontal collision to the left
                        // Player is touching from the left
                        std::cout << "Destroy Player, reset to start";
                        engine::ecs::EventManager::dispatcher.trigger(engine::ecs::PlayerDeath{ entityA });
                        //onPlayerDeath.invoke(); //TODO playerdeathevent
                    }else if(tagA == "obstacle" || tagB == "obstacle"){
                        std::cout << "Hit Obstacle, reset to start"; //TODO put logic in ContactListener!
                        engine::ecs::EventManager::dispatcher.trigger(engine::ecs::PlayerDeath{ entityA }); //TODO A oder B is player
                        //onPlayerDeath.invoke(); //TODO event in ContactListener
                    }
                    engine::ecs::EventManager::dispatcher.trigger(engine::ecs::PlayerGrounded{ entityA });

                }
            }
        }

    private:
        static void* getUserDataOfShape(const b2ShapeId& shape) {
            const b2BodyId body = b2Shape_GetBody(shape);
            if (!b2Body_IsValid(body)) return nullptr;
            return b2Body_GetUserData(body);
        }

    };
}
