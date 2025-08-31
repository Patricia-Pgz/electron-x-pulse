#include "PlayerInputSystem.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/levelloading/LevelManager.h"
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
            space_pressed = false;
        }

        if (velocity.y < 0.01f && velocity.y >= 0.f && can_jump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (!space_pressed)
            {
                space_pressed = true;
                applyJumpImpulse(body);
                can_jump = false;
            }
        }
        else if (glfwGetKey(game.getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE)
        {
            //reset when key released
            space_pressed = false;
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
            transform.zRotation += rotation_speed * game.getDeltaTime() * -y_gravity_multiplier;
        }
    }

    void PlayerInputSystem::onGravityChange(const engine::ecs::GravityChange& event)
    {
        if(B2_ID_EQUALS(event.gravityChangerID, previousGravityChanger))return;

        driving_on_ceiling = !driving_on_ceiling;
        y_gravity_multiplier = !driving_on_ceiling? -1.f : 1.f;
        b2World_SetGravity(game.getPhysicsWorld(), b2Vec2(0.0f, 10 * y_gravity_multiplier));

        const auto body = game.getRegistry().get<engine::ecs::PhysicsComponent>(game.getPlayer()).body;
        auto& transform = game.getRegistry().get<engine::ecs::TransformComponent>(game.getPlayer());

        b2Body_SetLinearVelocity(body, {0.f, 0.f});
        b2Body_ApplyLinearImpulseToCenter(body, {0.f, 0.5f * y_gravity_multiplier}, true);
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayerJump{true});

        if (driving_on_ceiling) {
            transform.scale.y = -std::abs(transform.scale.y); // flip vertically
        } else {
            transform.scale.y = std::abs(transform.scale.y);
        }
    }

    void PlayerInputSystem::applyJumpImpulse(const b2BodyId body) const
    {
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayerJump{true});

        const float desiredTimeToLand = game.getAudioSystem()->getConfig()->seconds_per_beat * landing_beats_ahead / engine::levelLoading::LevelManager::getCurrentLevel()->velocityMultiplier;
        const float t = desiredTimeToLand * 0.5f; // time to apex

        const float gravityY = (2.0f * desired_jump_height) / (t * t);
        // custom gravity to be able to choose jump height and jump length in time
        const float jumpVelocity = gravityY * t; // Initial vertical velocity
        const float mass = b2Body_GetMass(body);

        const float impulse = mass * jumpVelocity;

        // Set the world's gravity:
        b2World_SetGravity(game.getPhysicsWorld(), b2Vec2(0.0f, gravityY * y_gravity_multiplier));

        b2Body_ApplyLinearImpulseToCenter(body, b2Vec2(0.0f, impulse * -y_gravity_multiplier), true);
    }


    void PlayerInputSystem::onLvlLengthCompute(const engine::ecs::LevelLengthComputed& event)
    {
        curr_lvl_speed = event.levelSpeed;
    }

    void PlayerInputSystem::onReloadLevel()
    {
        driving_on_ceiling = false;
        y_gravity_multiplier = -1;
        previousGravityChanger = b2_nullShapeId;
        can_jump = true;
        space_pressed = false;

        b2World_SetGravity(game.getPhysicsWorld(), b2Vec2(0.0f, -10.0f));
    }
} // gl3
