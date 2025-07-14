#pragma once
#include "PlayerContactListener.h"
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

        ~PhysicsSystem() override
        {
            ecs::EventDispatcher::dispatcher.sink<ecs::PlayerJump>().disconnect<&
                PhysicsSystem::onPlayerJump>(this);
        }


        using event_t = events::Event<PhysicsSystem>;
        event_t onAfterPhysicsStep;

        /**
         * Runs the physics step, checks for player collisions/contacts, and moves entities based on their physics bodies, if they did not move out of the left border of the window.
         */
        void runPhysicsStep()
        {
            if (!game_.getRegistry().valid(game_.getPlayer()) || !is_active) return;
            accumulator += game_.getDeltaTime();
            if (accumulator >= fixed_time_step)
            {
                const b2WorldId world = game_.getPhysicsWorld();
                b2World_Step(world, fixed_time_step, sub_step_count);
                PlayerContactListener::checkForPlayerCollision(game_.getRegistry(), game_.getPlayer(),
                                                               world);

                updateGroupObjects();
                //update entities based on physics step
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
                    auto& transformComp = entities.get<ecs::TransformComponent>(entity);

                    //Check if most right point of object is still in window
                    if (transformComp.position.x + transformComp.scale.x * 0.5f < game_.getContext().
                        getWorldWindowBounds()[0])
                    {
                        b2Body_SetAwake(physics_comp.body, false);
                        physics_comp.isActive = false;
                    }
                    else
                    {
                        auto [p, q] = b2Body_GetTransform(physics_comp.body);
                        transformComp.position.x = p.x;
                        transformComp.position.y = p.y;
                    }
                }

                onAfterPhysicsStep.invoke();

                //wait for physics step, before setting player grounded from other classes/events
                if (player_jump_this_frame)
                {
                    PlayerContactListener::playerGrounded = !player_jump_this_frame;
                    player_jump_this_frame = false;
                }

                processDeletions();
                accumulator -= fixed_time_step;
            }
        };

        /**
         * Updates grouped objects safely after the physics step, to not interfere with registry changes.
         */
        void updateGroupObjects() const
        {
            //update grouped objects based on parent physics body
            auto& registry = game_.getRegistry();
            auto groupObjs = registry.view<ecs::TransformComponent, ecs::ParentComponent>();

            for (auto obj : groupObjs)
            {
                auto& parentComp = groupObjs.get<ecs::ParentComponent>(obj);
                if (!registry.valid(parentComp.parentEntity)) return;
                auto parentBody = registry.get<ecs::PhysicsComponent>(parentComp.parentEntity).body;
                auto parentPos = b2Body_GetPosition(parentBody);

                auto& transform = groupObjs.get<ecs::TransformComponent>(obj);
                transform.position = {
                    parentPos.x + parentComp.localOffset.x, parentPos.y + parentComp.localOffset.y, 0.f
                };
            }
        }

        void markBodyForDeletion(const b2BodyId body)
        {
            if (b2Body_IsValid(body))
            {
                bodies_to_delete.push_back(body);
            }
        }

        /**
         *Generates the GameObject (position & scale) for an AABB for grouped objects. For later use to instantiate a parent entity as Physics body for all its children.
         * @param objects Objects to group under one placeholder parent.
         * @return An AABB parent GameObject to generate an entity from to add as part of a ParentComponent to the grouped objects.
         */
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
        const float fixed_time_step = 1.0f / 60.0f;
        const int sub_step_count = 4;
        float accumulator = 0.f;
        bool player_jump_this_frame = false;

        std::vector<b2BodyId> bodies_to_delete;

        /**
         *Safely deletes the marked physics bodies and shapes after finishing the physics step.
         */
        void processDeletions()
        {
            for (const auto body : bodies_to_delete)
            {
                if (b2Body_IsValid(body))
                {
                    b2DestroyBody(body);
                }
            }
            bodies_to_delete.clear();
        }

        void onPlayerJump(const ecs::PlayerJump& event)
        {
            player_jump_this_frame = event.grounded;
        }
    };
} // gl3
