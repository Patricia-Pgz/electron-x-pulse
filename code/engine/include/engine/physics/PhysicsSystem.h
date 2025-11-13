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
            if (!is_active || !game.getRegistry().valid(game.getPlayer()))
                return;

            accumulator += game.getDeltaTime();
            if (accumulator < FIXED_TIME_STEP)
                return;

            auto& registry = game.getRegistry();
            const b2WorldId world = game.getPhysicsWorld();

            // Physics Step
            b2World_Step(world, FIXED_TIME_STEP, SUB_STEP_COUNT);
            PlayerContactListener::checkForPlayerCollision(registry, game.getPlayer(), world);

            const float leftBound = game.getContext().getWorldWindowBounds()[0];

            // Sync physics and parent components
            const auto phyView = registry.view<
                ecs::TransformComponent,
                ecs::TagComponent,
                ecs::PhysicsComponent
            >();

            for (const auto entity : phyView)
            {
                auto& tc = phyView.get<ecs::TransformComponent>(entity);
                auto& pc = phyView.get<ecs::PhysicsComponent>(entity);

                if (!pc.isActive || !b2Body_IsValid(pc.body))
                    continue;

                if (registry.any_of<ecs::PhysicsGroupParent>(entity))
                {
                    updateParentPhysics(tc, pc);
                    continue;
                }

                updateStandardPhysics(tc, pc, leftBound);
            }

            // Sync group children
            const auto childView = registry.view<
                ecs::PhysicsGroupChild,
                ecs::TransformComponent
            >();

            for (auto [child, physChild, childTC] : childView.each())
            {
                updateChildEntity(physChild, childTC, registry, leftBound);
            }

            // Cleanup
            onAfterPhysicsStep.invoke();

            if (player_jump_this_frame)
            {
                PlayerContactListener::playerGrounded = false;
                player_jump_this_frame = false;
            }

            processDeletions();
            accumulator -= FIXED_TIME_STEP;
        }

        /**
         * Utility: Rotate Child Transform according to Parent.
         * @param offset The child's local offset to the Parent.
         * @param rotationDeg The Parent's (new) zRotation.
         * @return
         */
        static glm::vec3 rotatedOffset(const glm::vec2& offset, const float rotationDeg)
        {
            const float rot = glm::radians(rotationDeg);

            return {
                offset.x * cos(rot) - offset.y * sin(rot),
                offset.x * sin(rot) + offset.y * cos(rot),
                0.f
            };
        }

        /**
         * @brief Updates a group parent according to the physics step.
         * @param tc TransformComponent of the parent entity.
         * @param pc PhysicsComponent of the parent entity.
         */
        static void updateParentPhysics(
            ecs::TransformComponent& tc,
            const ecs::PhysicsComponent& pc)
        {
            b2Body_SetAwake(pc.body, true);
            auto [p, q] = b2Body_GetTransform(pc.body);

            tc.position.x = p.x;
            tc.position.y = p.y;
        }

        /**
         * @brief Updates entities according to the physics step.
         * @param tc TransformComponent of the entity.
         * @param pc PhysicsComponent of the entity.
         * @param leftBound Left window bound for visibility check.
         */
        static void updateStandardPhysics(
            ecs::TransformComponent& tc,
            const ecs::PhysicsComponent& pc,
            const float leftBound)
        {
            const float rightEdge = tc.position.x + tc.scale.x * 0.5f;
            const bool visible = rightEdge >= leftBound;

            b2Body_SetAwake(pc.body, visible);

            if (visible)
            {
                auto [p, q] = b2Body_GetTransform(pc.body);
                tc.position.x = p.x;
                tc.position.y = p.y;
            }
        }

        /**
         * @brief Updates group children according to their parent
         * @param physChild PhysicsGroupChild component reference of the child entity.
         * @param childTC TransformComponent of the child.
         * @param registry The current EnTT Registry.
         * @param leftBound The left window bound for visibility checking.
         */
        static void updateChildEntity(
            ecs::PhysicsGroupChild& physChild,
            ecs::TransformComponent& childTC,
            entt::registry& registry,
            const float leftBound)
        {
            if (!registry.valid(physChild.root)) return;

            if (!registry.all_of<ecs::TransformComponent, ecs::PhysicsComponent>(physChild.root))
                return;

            const auto& parentTC = registry.get<ecs::TransformComponent>(physChild.root);
            auto& parentPhysComp = registry.get<ecs::PhysicsComponent>(physChild.root);

            if (!parentPhysComp.isActive)
                return;

            if (const float rightEdge = childTC.position.x + childTC.scale.x * 0.5f; rightEdge >= leftBound)
            {
                const glm::vec3 offset = rotatedOffset(physChild.localOffset, parentTC.zRotation);
                childTC.position = parentTC.position + offset;
                childTC.zRotation = parentTC.zRotation;
                return;
            }

            if (physChild.isActive)
            {
                auto& [bodyID, childCount, visibleChildren] = registry.get<ecs::PhysicsGroupParent>(physChild.root);
                --visibleChildren;
                physChild.isActive = false;

                if (visibleChildren <= 0)
                {
                    parentPhysComp.isActive = false;
                }
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

        static GameObject computeGroupAABB(const std::vector<entt::entity>& entities, entt::registry& registry)
        {
            if (entities.empty())
            {
                return {};
            }


            // Transforms defining the bounds
            ecs::TransformComponent& leftMost = registry.get<ecs::TransformComponent>(entities[0]);
            ecs::TransformComponent& rightMost = leftMost;
            ecs::TransformComponent& topMost = leftMost;
            ecs::TransformComponent& bottomMost = leftMost;

            for (const auto& entity : entities)
            {
                const auto& transform = registry.get<ecs::TransformComponent>(entity);
                if (transform.position.x - transform.scale.x * 0.5f < leftMost.position.x - leftMost.scale.x * 0.5f)
                {
                    leftMost = transform;
                }
                if (transform.position.x + transform.scale.x * 0.5f > rightMost.position.x + rightMost.scale.x *
                    0.5f)
                {
                    rightMost = transform;
                }
                if (transform.position.y + transform.scale.y * 0.5f > topMost.position.y + topMost.scale.y * 0.5f)
                {
                    topMost = transform;
                }
                if (transform.position.y - transform.scale.y * 0.5f < bottomMost.position.y - bottomMost.scale.y *
                    0.5f)
                {
                    bottomMost = transform;
                }
            }

            const float left = leftMost.position.x - leftMost.scale.x * 0.5f;
            const float right = rightMost.position.x + rightMost.scale.x * 0.5f;
            const float top = topMost.position.y + topMost.scale.y * 0.5f;
            const float bottom = bottomMost.position.y - bottomMost.scale.y * 0.5f;

            GameObject result;
            result.position = {(left + right) * 0.5f, (top + bottom) * 0.5f, 0.f};
            result.scale = {right - left, top - bottom, 0.f};

            return result;
        }

    private:
        static constexpr float FIXED_TIME_STEP = 1.0f / 60.0f; ///< Fixed physics timestep (60Hz)
        static constexpr int SUB_STEP_COUNT = 4; ///< Number of Box2D sub-steps per physics step
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
