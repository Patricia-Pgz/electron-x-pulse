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

    void PlayerInputSystem::update(const entt::entity& player)
    {
        const auto window = game.getWindow();
        const auto body = game.getRegistry().get<engine::ecs::PhysicsComponent>(player).body;
        b2Vec2 velocity = b2Body_GetLinearVelocity(body);

        if (velocity.y < 0.01f && velocity.y >= 0.f && canJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            applyJumpImpulse(body);
            canJump = false;
        }
    }


    void PlayerInputSystem::applyJumpImpulse(const b2BodyId body) const
    {
        const b2Vec2 jumpImpulse = calculateJumpImpulse(
            body, JumpConfig(9.81, game.getCurrentConfig().bpm, distancePerBeat));
        b2Body_ApplyLinearImpulseToCenter(body, jumpImpulse, true);
    }

    void PlayerInputSystem::onPlayerGrounded(engine::ecs::PlayerGrounded& event)
    {
        canJump = true;
    }
} // gl3
