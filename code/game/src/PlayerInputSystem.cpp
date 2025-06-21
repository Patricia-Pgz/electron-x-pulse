#include "PlayerInputSystem.h"
#include "engine/ecs/EntityFactory.h"

namespace gl3::game::input
{
    //TODO Wie will ich Jumpimpuls jetzt wirklich berechnen?
    b2Vec2 PlayerInputSystem::calculateJumpImpulse(const b2BodyId body, const JumpConfig& config)
    {
        const float distancePerBeat = 60.0f / config.bpm;
        const float jumpDuration = config.beatsPerJump * distancePerBeat;
        const float initialVelocity = (config.gravity * jumpDuration) / 2.0f;
        const float bodyMass = b2Body_GetMass(body);
        const b2Vec2 jumpImpulse(0.0f, initialVelocity * bodyMass);

        return jumpImpulse;
    }

    void PlayerInputSystem::update()
    {
        if (!game_.getRegistry().valid(game_.getPlayer()) || !is_active) return;
        const auto window = game_.getWindow();
        const auto body = game_.getRegistry().get<engine::ecs::PhysicsComponent>(game_.getPlayer()).body;
        b2Vec2 velocity = b2Body_GetLinearVelocity(body);

        if (velocity.y < 0.01f && velocity.y >= 0.f && canJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            applyJumpImpulse(body);
            canJump = false;
        }

        const float fixedX = game_.getRegistry().get<engine::ecs::TransformComponent>(game_.getPlayer()).initialPosition.x;
        b2Vec2 pos = b2Body_GetPosition(body);
        pos.x = fixedX;
        b2Body_SetTransform(body, pos, b2Body_GetRotation(body));

        b2Vec2 vel = b2Body_GetLinearVelocity(body);
        vel.x = 0.0f;
        b2Body_SetLinearVelocity(body, vel);
    }


    void PlayerInputSystem::applyJumpImpulse(const b2BodyId body) const
    {
        /*const b2Vec2 jumpImpulse = calculateJumpImpulse(
            body, JumpConfig(9.81, game.getCurrentConfig().bpm, distancePerBeat));*/
        b2Body_ApplyLinearImpulseToCenter(body, {0.f, 0.5f}, true);
        //TODO so berechnen, dass player am höchsten  punkt distanceperbeat hoch hüpft?
    }

    void PlayerInputSystem::onPlayerGrounded(engine::ecs::PlayerGrounded& event)
    {
        canJump = true;
    }
} // gl3
