#include "PlayerInputSystem.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/physics/PlayerContactListener.h"
#include "glm/gtc/constants.hpp"

namespace gl3::game::input
{
    void PlayerInputSystem::update()
    {
        if (!game.getRegistry().valid(game.getPlayer()) || !is_active) return;
        const auto window = game.getWindow();
        const auto body = game.getRegistry().get<engine::ecs::PhysicsComponent>(game.getPlayer()).body;
        auto& transform = game.getRegistry().get<engine::ecs::TransformComponent>(game.getPlayer());
        b2Vec2 velocity = b2Body_GetLinearVelocity(body);

        if (engine::physics::PlayerContactListener::playerGrounded)
        {
            can_jump = true;
            enter_pressed = false;
        }

        if (velocity.y < 0.01f && velocity.y >= 0.f && can_jump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (!enter_pressed)
            {
                enter_pressed = true;
                applyJumpImpulse(body);
                can_jump = false;
            }
        }
        else if (glfwGetKey(game.getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            //reset when key released
            enter_pressed = false;
        }

        const float fixedX = game.getRegistry().get<engine::ecs::TransformComponent>(game.getPlayer()).initialPosition
                                 .x;
        b2Vec2 pos = b2Body_GetPosition(body);
        pos.x = fixedX;
        b2Body_SetTransform(body, pos, b2Body_GetRotation(body));

        b2Vec2 vel = b2Body_GetLinearVelocity(body);
        vel.x = 0.0f;
        b2Body_SetLinearVelocity(body, vel);

        if (engine::physics::PlayerContactListener::playerGrounded)
        {
            // smoothly stop visual player rotation
            float& angle = transform.zRotation;
            angle = glm::mix(angle, targetRotation, game.getDeltaTime() * 15);
            angle = glm::mod(angle, glm::two_pi<float>());

            if (std::abs(angle - targetRotation) < 0.01f)
            {
                angle = targetRotation;
            }
        }
        else
        {
            // still spin freely when in air
            transform.zRotation += rotation_speed * game.getDeltaTime();
        }
    }

    void PlayerInputSystem::onGravityChange(engine::ecs::GravityChange& event)
    {
        // let the player fly to the ceiling
        change_jump_mechanics = !change_jump_mechanics;
        y_gravity_multiplier = change_jump_mechanics ? -1 : 1;
        b2World_SetGravity(game.getPhysicsWorld(), b2Vec2(0.0f, -10 * y_gravity_multiplier));
        const auto body = game.getRegistry().get<engine::ecs::PhysicsComponent>(game.getPlayer()).body;
        auto & transform = game.getRegistry().get<engine::ecs::TransformComponent>(game.getPlayer());

        b2Body_ApplyLinearImpulseToCenter(body,{0.f, 0.5f}, true);
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayerJump{true});

        if (change_jump_mechanics) {
            transform.scale.y = -std::abs(transform.scale.y); // flip vertically for moving on ceiling
        } else {
            transform.scale.y = std::abs(transform.scale.y);
        }
    }

    void PlayerInputSystem::applyJumpImpulse(const b2BodyId body) const
    {
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayerJump{true});

        const float desiredTimeToLand = game.getAudioSystem()->getConfig()->seconds_per_beat * landing_beats_ahead;
        const float t = desiredTimeToLand * 0.5f; // time to apex

        const float gravityY = (2.0f * desired_jump_height) / (t * t) * y_gravity_multiplier;
        // custom gravity to be able to choose jump height and jump length in time
        const float jumpVelocity = gravityY * t; // Initial vertical velocity
        const float mass = b2Body_GetMass(body);

        const float impulse = mass * jumpVelocity;

        // Set the world's gravity:
        b2World_SetGravity(game.getPhysicsWorld(), b2Vec2(0.0f, -gravityY));

        b2Body_ApplyLinearImpulseToCenter(body, b2Vec2(0.0f, impulse), true);
    }


    void PlayerInputSystem::onLvlLengthCompute(const engine::ecs::LevelLengthComputed& event)
    {
        curr_lvl_speed = event.levelSpeed;
    }

    void PlayerInputSystem::onReloadLevel()
    {
        change_jump_mechanics = false;
        y_gravity_multiplier = 1;

        b2World_SetGravity(game.getPhysicsWorld(), b2Vec2(0.0f, -10.0f));
    }
} // gl3
