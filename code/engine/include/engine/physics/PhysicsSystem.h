#pragma once
#include "PlayerContactListener.h"
#include "engine/Constants.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"
#include "engine/Game.h"
#include <box2d/box2d.h>

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
                    auto [x, y] = b2Body_GetPosition(physics_comp.body);
                    auto& transformComp = entities.get<ecs::TransformComponent>(entity);

                    //Check if most right point of object is still in window
                    /*if (game_.getContext().getScreenPosObjBounds({x, y}, {transformComp.scale.x, transformComp.scale.y})
                        [1].x <=)
                    {
                        b2Body_SetAwake(physics_comp.body, false);
                        physics_comp.isActive = false;
                    }
                    else*/
                    {
                        auto [p, q] = b2Body_GetTransform(physics_comp.body);
                        transformComp.position.x = p.x;
                        transformComp.position.y = p.y;
                    }
                }

                //Wait for physics step, before setting player grounded from other classes/events
                if (player_jump_this_frame)
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

        static GameObject computeGroupAABB(const std::vector<GameObject>& objects)
        {
            if (objects.empty())
            {
                return {};
            }

            // Pointers to the objects defining the bounds
            const GameObject* leftMost = nullptr;
            const GameObject* rightMost = nullptr;
            const GameObject* topMost = nullptr;
            const GameObject* bottomMost = nullptr;

            for (const auto& obj : objects)
            {
                if (!leftMost || obj.position.x - obj.scale.x * 0.5f < leftMost->position.x - leftMost->scale.x * 0.5f)
                {
                    leftMost = &obj;
                }
                if (!rightMost || obj.position.x + obj.scale.x * 0.5f > rightMost->position.x + rightMost->scale.x *
                    0.5f)
                {
                    rightMost = &obj;
                }
                if (!topMost || obj.position.y + obj.scale.y * 0.5f > topMost->position.y + topMost->scale.y * 0.5f)
                {
                    topMost = &obj;
                }
                if (!bottomMost || obj.position.y - obj.scale.y * 0.5f < bottomMost->position.y - bottomMost->scale.y *
                    0.5f)
                {
                    bottomMost = &obj;
                }
            }

            if (!leftMost || !rightMost || !topMost || !bottomMost) return {};

            const float left = leftMost->position.x - leftMost->scale.x * 0.5f;
            const float right = rightMost->position.x + rightMost->scale.x * 0.5f;
            const float top = topMost->position.y + topMost->scale.y * 0.5f;
            const float bottom = bottomMost->position.y - bottomMost->scale.y * 0.5f;

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
