#pragma once
#include "ContactListener.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"

namespace gl3
{
    class PhysicsSystem : public engine::ecs::System
    {
    public:
        explicit PhysicsSystem(engine::Game& game) : System(game){};

        void runPhysicsStep()
        {
            accumulator += game.getDeltaTime();
            if (accumulator >= fixedTimeStep)
            {
                const b2WorldId world = game.getPhysicsWorld();
                b2World_Step(world, fixedTimeStep, subStepCount);
                ContactListener::checkForCollision(world);

                if (game.currentGameState == engine::GameState::PreviewWithScrolling) return;

                // Update the entities based on what happened in the physics step
                const auto& entities = game.getRegistry().view<engine::ecs::TagComponent, engine::ecs::TransformComponent,
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

                    if(tag_comp.tag == "player")
                    {
                        b2Vec2 position = b2Body_GetPosition(physics_comp.body);
                        position.x = transform_comp.position.x; // Force the X position to remain constant (locked)
                        b2Body_SetTransform(physics_comp.body, position, b2Body_GetRotation(physics_comp.body));
                    } else
                    {
                        transform_comp.position.x = p.x; //TODO evtl solche Dinge nur tun, wenn physics comp awake?
                        transform_comp.position.y = p.y;

                    }
                    transform_comp.zRotation = glm::degrees(b2Rot_GetAngle(q));

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
