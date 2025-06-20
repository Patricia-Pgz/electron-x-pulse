#pragma once
#include "PlayerContactListener.h"
#include "engine/Constants.h"
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
            if (game.getPlayer() == entt::null || !is_active) return;
            accumulator += game.getDeltaTime();
            if (accumulator >= fixedTimeStep)
            {
                const b2WorldId world = game.getPhysicsWorld();
                b2World_Step(world, fixedTimeStep, subStepCount);
                PlayerContactListener::checkForPlayerCollision(game.getRegistry(), game.getPlayer());

                if (game.currentGameState == GameState::PreviewWithScrolling) return;

                // Update entities based on physics step
                const auto& entities = game.getRegistry().view<
                    ecs::TagComponent, ecs::TransformComponent,
                    ecs::PhysicsComponent>();

                for (auto& entity : entities)
                {
                    auto& physics_comp = entities.get<ecs::PhysicsComponent>(entity);
                    if (!b2Body_IsValid(physics_comp.body) || !physics_comp.isActive)
                    {
                        continue;
                    }

                    if (auto [x, y] = b2Body_GetPosition(physics_comp.body); x <= game.getContext().getWindowBounds()[0]
                        - 1 * pixelsPerMeter)
                    {
                        b2Body_SetAwake(physics_comp.body, false);
                        physics_comp.isActive = false;
                    }

                    auto& transformComp = entities.get<ecs::TransformComponent>(entity);
                    auto [p, q] = b2Body_GetTransform(physics_comp.body);
                    transformComp.position.x = p.x;
                    transformComp.position.y = p.y;
                    transformComp.zRotation = glm::degrees(b2Rot_GetAngle(q));
                }

                accumulator -= fixedTimeStep;
            }
        };

    private:
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 4;
        float accumulator = 0.f;

        static void onPlayerGrounded();
    };
} // gl3
