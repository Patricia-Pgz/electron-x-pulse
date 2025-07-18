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
            canJump = true;
            enter_pressed_ = false;
        }

        if (velocity.y < 0.01f && velocity.y >= 0.f && canJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (!enter_pressed_)
            {
                enter_pressed_ = true;
                applyJumpImpulse(body);
                canJump = false;
            }
        }
        else if (glfwGetKey(game.getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            enter_pressed_ = false;
        }

        const float fixedX = game.getRegistry().get<engine::ecs::TransformComponent>(game.getPlayer()).initialPosition
                                  .x;
        b2Vec2 pos = b2Body_GetPosition(body);
        pos.x = fixedX;
        b2Body_SetTransform(body, pos, b2Body_GetRotation(body));

        b2Vec2 vel = b2Body_GetLinearVelocity(body);
        vel.x = 0.0f;
        b2Body_SetLinearVelocity(body, vel);
        if (!engine::physics::PlayerContactListener::playerGrounded)
        {
            //player animation
            transform.zRotation += rotationSpeed * game.getDeltaTime();
        }
        else
        {
            if (engine::physics::PlayerContactListener::playerGrounded)
            {
                //smoothly stop rotation
                float& angle = transform.zRotation;
                angle = glm::mix(angle, 0.0f, game.getDeltaTime() * 15); // smooth angle
                angle = glm::mod(angle, glm::two_pi<float>());
                if (std::abs(angle) < 0.01f)
                {
                    angle = 0.0f;
                }
            }
        }
    }


    void PlayerInputSystem::applyJumpImpulse(const b2BodyId body)
    {
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayerJump{true});
        const float gravityY = std::abs(b2World_GetGravity(game.getPhysicsWorld()).y);
        const float mass = b2Body_GetMass(body);

        const float timeToLand = game.getAudioSystem()->getConfig()->seconds_per_beat;


        // Time to apex: total time / 2
        const float t = timeToLand * 0.5f;

        // The required initial velocity to reach that height in t
        const float jumpVelocity = (desiredJumpHeight * jumpHeightFactor + 0.5f * gravityY * t * t) / t;

        const float impulse = mass * jumpVelocity;

        b2Body_ApplyLinearImpulseToCenter(body, b2Vec2(0.0f, impulse), true);
    }

    void PlayerInputSystem::onLvlLengthCompute(const engine::ecs::LevelLengthComputed& event)
    {
        curr_lvl_speed = event.levelSpeed;
    }
} // gl3
