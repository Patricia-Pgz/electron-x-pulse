#pragma once
#include "ContactListener.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"

namespace gl3
{
    class PhysicsSystem : public engine::ecs::System
    {
    public:
        explicit PhysicsSystem(engine::Game& game) : System(game)
        {
        };

        void runPhysicsStep(b2WorldId physicsWorld, float deltaTime, entt::registry& registry)
        {
            accumulator += deltaTime;
            if (accumulator >= fixedTimeStep)
            {
                b2World_Step(physicsWorld, fixedTimeStep, subStepCount);
                ContactListener::checkForCollision(physicsWorld);

                if (game.currentGameState == engine::GameState::PreviewWithScrolling) return;

                // Update the entities based on what happened in the physics step
                const auto& entities = registry.view<engine::ecs::TagComponent, engine::ecs::TransformComponent,
                                              engine::ecs::PhysicsComponent>();

                for (auto& entity : entities)
                {
                    auto& tag_comp = entities.get<engine::ecs::TagComponent>(entity);
                    auto& transform_comp = entities.get<engine::ecs::TransformComponent>(entity);
                    auto& physics_comp = entities.get<engine::ecs::PhysicsComponent>(entity);

                    if (tag_comp.tag == "timeline") continue;

                    if (tag_comp.tag == "beat")
                    {
                        // Update position based on scroll speed and deltaTime
                        transform_comp.position.x += game.getLevelSpeed() * fixedTimeStep;
                        continue;
                    }

                    if (!b2Body_IsValid(physics_comp.body))
                        return;

                    auto [p, q] = b2Body_GetTransform(physics_comp.body);

                    transform_comp.position.x = p.x;
                    transform_comp.position.y = p.y;

                    transform_comp.zRotation = glm::degrees(b2Rot_GetAngle(q));

                    // If entity is off-screen, sleep to save performance //TODO das ist unnötig, da die velocity die bodies eh direkt wieder aufweckt!
                    /*if (b2Body_GetPosition(physics_comp.body).x < game.getContext().getWindowBounds()[0] - 1.0f)
                    // Arbitrary value to check if the entity is far off-screen
                    {
                        b2Body_SetAwake(physics_comp.body, false); // Sleep the body
                    }
                    else
                    {
                        b2Body_SetAwake(physics_comp.body, true); // Wake the body if it’s back on screen
                    }*/
                }

                accumulator -= fixedTimeStep;
            }
        };

    private:
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 8; // recommended sub-step count
        float accumulator = 0.f;
    };
} // gl3
