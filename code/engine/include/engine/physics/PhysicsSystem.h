#pragma once
#include "PlayerContactListener.h"
#include "engine/Constants.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"
#include "engine/Game.h"
#include <box2d/box2d.h>
#include "../../../../extern/box2d/src/body.h"

namespace gl3::engine::physics
{
    class PhysicsSystem final : public ecs::System
    {
    public:
        explicit PhysicsSystem(Game& game) : System(game)
        {
            ecs::EventDispatcher::dispatcher.sink<ecs::PlayerJump>().connect<&
                PhysicsSystem::onPlayerJump>(this);
        };

        void runPhysicsStep()
        {
            if (!game_.getRegistry().valid(game_.getPlayer()) || !is_active) return;
            accumulator += game_.getDeltaTime();
            if (accumulator >= fixedTimeStep)
            {
                const b2WorldId world = game_.getPhysicsWorld();
                b2World_Step(world, fixedTimeStep, subStepCount);
                PlayerContactListener::checkForPlayerCollision(game_.getRegistry(), game_.getPlayer(),
                                                               game_.getPhysicsWorld());

                if (game_.currentGameState == GameState::PreviewWithScrolling) return;

                // Update entities based on physics step
                const auto& entities = game_.getRegistry().view<
                    ecs::TagComponent, ecs::TransformComponent,
                    ecs::PhysicsComponent>();

                for (auto& entity : entities)
                {
                    auto& physics_comp = entities.get<ecs::PhysicsComponent>(entity);
                    if (!b2Body_IsValid(physics_comp.body) || !physics_comp.isActive)
                    {
                        continue;
                    }

                    if (auto [x, y] = b2Body_GetPosition(physics_comp.body); x <= game_.getContext().getWindowBounds()[
                            0]
                        - 1 * pixelsPerMeter)
                    {
                        b2Body_SetAwake(physics_comp.body, false);
                        physics_comp.isActive = false;
                    }

                    auto& transformComp = entities.get<ecs::TransformComponent>(entity);
                    auto [p, q] = b2Body_GetTransform(physics_comp.body);
                    transformComp.position.x = p.x;
                    transformComp.position.y = p.y;
                }
                if (player_jump_this_frame)
                //Wait for physics step, before setting player grounded from other classes/events
                {
                    PlayerContactListener::playerGrounded = !player_jump_this_frame;
                    player_jump_this_frame = false;
                }
                processDeletions();
                accumulator -= fixedTimeStep;
            }
        };

        void markBodyForDeletion(const b2BodyId body)
        {
            if (b2Body_IsValid(body))
            {
                bodiesToDelete.push_back(body);
            }
        }

        static GameObject computeGroupAABB(const entt::registry& registry, const std::vector<entt::entity>& objects)
        {
            if (objects.empty())
            {
                return {};
            }

            entt::entity leftMost = entt::null;
            entt::entity rightMost = entt::null;
            entt::entity topMost = entt::null;
            entt::entity bottomMost = entt::null;
            ecs::TransformComponent transformMostLeft = entt::null;
            ecs::TransformComponent transformMostRight = entt::null;
            ecs::TransformComponent transFormTop = entt::null;
            ecs::TransformComponent transformBot = entt::null;

            for (const auto& entity : objects)
            {
                auto transform = registry.get<ecs::TransformComponent>(entity);
                transformMostLeft = registry.get<ecs::TransformComponent>(leftMost);
                transformMostRight = registry.get<ecs::TransformComponent>(rightMost);
                transFormTop = registry.get<ecs::TransformComponent>(topMost);
                transformBot = registry.get<ecs::TransformComponent>(bottomMost);
                if (leftMost == entt::null || transform.position.x - transform.scale.x * 0.5f < transformMostLeft.
                    position.x - transformMostLeft.scale.x * 0.5f)
                {
                    leftMost = entity;
                }
                if (rightMost == entt::null || transform.position.x + transform.scale.x * 0.5f > transformMostRight.
                    position.x + transformMostRight.scale.x * 0.5f)
                {
                    rightMost = entity;
                }
                if (topMost == entt::null || transform.position.y + transform.scale.y * 0.5f > transFormTop.position.y +
                    transFormTop.scale.y * 0.5f)
                {
                    topMost = entity;
                }
                if (bottomMost == entt::null || transform.position.y - transform.scale.y * 0.5f < transformBot.position.
                    y - transformBot.scale.y *
                    0.5f)
                {
                    bottomMost = entity;
                }
            }

            const float left = transformMostLeft.position.x - transformMostLeft.scale.x * 0.5f;
            const float right = transformMostRight.position.x + transformMostRight.scale.x * 0.5f;
            const float top = transFormTop.position.y + transFormTop.scale.y * 0.5f;
            const float bottom = transformBot.position.y - transformBot.scale.y * 0.5f;

            GameObject result;
            result.position = {(left + right) * 0.5f, (top + bottom) * 0.5f, 0.f};
            result.scale = {right - left, top - bottom, 0.f};

            return result;
        }

    private:
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 4;
        float accumulator = 0.f;
        bool player_jump_this_frame = false;

        std::vector<b2BodyId> bodiesToDelete;

        void processDeletions()
        {
            for (const auto body : bodiesToDelete)
            {
                if (b2Body_IsValid(body))
                {
                    b2DestroyBody(body);
                }
            }
            bodiesToDelete.clear();
        }

        void onPlayerJump(const ecs::PlayerJump& event)
        {
            player_jump_this_frame = event.grounded;
        }
    };
} // gl3
