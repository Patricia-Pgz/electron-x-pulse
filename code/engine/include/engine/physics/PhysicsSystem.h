#pragma once
#include "PlayerContactListener.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"
#include "engine/Game.h"

namespace gl3::engine::physics
{
    class PhysicsSystem : public ecs::System
    {
    public:
        explicit PhysicsSystem(Game& game) : System(game)
        {
        };

        void runPhysicsStep()
        {
            accumulator += game.getDeltaTime();
            if (accumulator >= fixedTimeStep)
            {
                const b2WorldId world = game.getPhysicsWorld();
                b2World_Step(world, fixedTimeStep, subStepCount);
                PlayerContactListener::checkForPlayerCollision(game.getRegistry(), game.getPlayer());

                if (game.currentGameState == engine::GameState::PreviewWithScrolling) return;

                // Update the entities based on what happened in the physics step
                const auto& entities = game.getRegistry().view<
                    engine::ecs::TagComponent, ecs::TransformComponent,
                    engine::ecs::PhysicsComponent>();

                for (auto& entity : entities)
                {
                    auto& transform_comp = entities.get<ecs::TransformComponent>(entity);
                    auto& physics_comp = entities.get<ecs::PhysicsComponent>(entity);
                    if (!b2Body_IsValid(physics_comp.body))
                        return;

                    auto [p, q] = b2Body_GetTransform(physics_comp.body);
                    transform_comp.position.x = p.x; //TODO physics comp enabled?
                    transform_comp.position.y = p.y;
                    transform_comp.zRotation = glm::degrees(b2Rot_GetAngle(q));
                }

                accumulator -= fixedTimeStep;
            }
        };

    private:
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 8; // recommended sub-step count
        float accumulator = 0.f;

        static void onPlayerGrounded();
    };
} // gl3
