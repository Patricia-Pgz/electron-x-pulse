#pragma once
#include "PlayerContactListener.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"
#include "engine/Game.h"
#include <box2d/box2d.h>

namespace gl3::engine::physics
{
    /**
     * @class PhysicsSystem
     * @brief Handles the physics simulation step using Box2D and integrates physics updates into the ECS (enTT).
     *
     * This system runs fixed-timestep physics simulation, updates entity transforms based on physics bodies,
     * manages player collision detection, and handles grouping of objects with parent-child relationships.
     * It also listens to player jump events to correctly update grounded state.
     */
    class PhysicsSystem final : public ecs::System
    {
    public:
        /**
         * @brief Constructs the PhysicsSystem and subscribes to player jump events.
         * @param game Reference to the main game instance.
         */
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

        /// Event triggered after the physics step completes
        using event_t = events::Event<PhysicsSystem>;
        event_t onAfterPhysicsStep;

        /**
         * @brief Advances the physics simulation by the fixed timestep if enough time has elapsed.
         *
         * - Steps the Box2D world simulation.
         * - Checks for player collisions and contacts.
         * - Updates transforms of entities with physics bodies.
         * - Deactivates bodies that move outside the left window boundary.
         * - Invokes after-step event and processes deletions of marked physics bodies.
         */
        void runPhysicsStep()
        {
            if (!game.getRegistry().valid(game.getPlayer()) || !is_active) return;
            accumulator += game.getDeltaTime();
            if (accumulator >= fixed_time_step)
            {
                const b2WorldId world = game.getPhysicsWorld();
                b2World_Step(world, fixed_time_step, sub_step_count);
                PlayerContactListener::checkForPlayerCollision(game.getRegistry(), game.getPlayer(),
                                                               world);

                updateGroupObjects();
                //update entities based on physics step
                const auto& entities = game.getRegistry().view<
                    ecs::TagComponent, ecs::TransformComponent,
                    ecs::PhysicsComponent>();

                for (auto& entity : entities)
                {
                    const auto& physics_comp = entities.get<ecs::PhysicsComponent>(entity);
                    if (!b2Body_IsValid(physics_comp.body) || !physics_comp.isActive)
                    {
                        continue;
                    }
                    auto& transformComp = entities.get<ecs::TransformComponent>(entity);

                    //Check if most right point of object is still in window
                    if (transformComp.position.x + transformComp.scale.x * 0.5f < game.getContext().
                        getWorldWindowBounds()[0])
                    {
                        b2Body_SetAwake(physics_comp.body, false);
                    }
                    else
                    {
                        b2Body_SetAwake(physics_comp.body, true);
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
         * @brief Updates positions of grouped child objects based on their parent physics body.
         *
         * Ensures that child transforms follow their parent's physics body with an offset.
         */
        void updateGroupObjects() const
        {
            //update grouped objects based on parent physics body
            auto& registry = game.getRegistry();
            const auto groupObjs = registry.view<ecs::TransformComponent, ecs::ParentComponent>();

            for (const auto obj : groupObjs)
            {
                const auto& parentComp = groupObjs.get<ecs::ParentComponent>(obj);
                if (!registry.valid(parentComp.parentEntity)) return;
                const auto parentBody = registry.get<ecs::PhysicsComponent>(parentComp.parentEntity).body;
                const auto parentPos = b2Body_GetPosition(parentBody);

                auto& transform = groupObjs.get<ecs::TransformComponent>(obj);
                transform.position = {
                    parentPos.x + parentComp.localOffset.x, parentPos.y + parentComp.localOffset.y, 0.f
                };
            }
        }

        /**
         * @brief Marks a Box2D body for safe deletion after the physics step.
         * @param body The Box2D body to delete.
         */
        void markBodyForDeletion(const b2BodyId body)
        {
            if (b2Body_IsValid(body))
            {
                bodies_to_delete.push_back(body);
            }
        }

        /**
         * @brief Computes an axis-aligned bounding box (AABB) GameObject that contains all given objects for physics grouping.
         *
         * Used to generate a parent GameObject encapsulating multiple child objects for physics grouping.
         * @param objects Vector of GameObjects to group.
         * @return A GameObject representing the combined AABB.
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
        const float fixed_time_step = 1.0f / 60.0f; ///< Fixed physics timestep (60Hz)
        const int sub_step_count = 4; ///< Number of Box2D sub-steps per physics step
        float accumulator = 0.f; ///< Accumulates elapsed time to run fixed timestep

        bool player_jump_this_frame = false; ///< Tracks if player jumped this frame to update grounded state

        std::vector<b2BodyId> bodies_to_delete; ///< Bodies scheduled for deletion after physics step

        /**
         * @brief Safely deletes all bodies marked for deletion after the physics step.
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

        /**
        * @brief Event handler called when player jump event is received.
        * @param event The player jump event containing grounded state.
        */
        void onPlayerJump(const ecs::PlayerJump& event)
        {
            player_jump_this_frame = event.grounded;
        }
    };
} // gl3
