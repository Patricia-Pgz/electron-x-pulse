#include "PlayerInputSystem.h"

#include <iostream>

#include "engine/ecs/EntityFactory.h"

namespace gl3 {

    b2Vec2 PlayerInputSystem::calculateJumpImpulse(const b2BodyId body, const JumpConfig& config) {
        float secondsPerBeat = 60.0f / static_cast<float>(config.bpm);

        float jumpDuration = config.beatsPerJump * secondsPerBeat;

        float initialVelocity = (config.gravity * jumpDuration) / 2.0f;

        float bodyMass = b2Body_GetMass(body);
        b2Vec2 jumpImpulse(0.0f, initialVelocity * bodyMass);

        return jumpImpulse;
    }

    void PlayerInputSystem::update(const entt::entity& player)
    {
        const auto window = game.getWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }


        const auto body = game.getRegistry().get<engine::ecs::PhysicsComponent>(player).body;
        b2Vec2 velocity = b2Body_GetLinearVelocity(body);

        bool canJump = (isPlayerGrounded && velocity.y < 0.01f);

        if (canJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) //TODO jumping funktioniert nicht immer? Death macht nur chaos
        {
            std::cout << "jump";
            applyJumpImpulse(body);
            isPlayerGrounded = false;
        }
    }


    void PlayerInputSystem::applyJumpImpulse(const b2BodyId body) const
    {
        const b2Vec2 jumpImpulse = calculateJumpImpulse(body, JumpConfig(9.81, game.getCurrentBPM(), distancePerBeat));
        std::cout << std::to_string(jumpImpulse.y);
        b2Body_ApplyLinearImpulseToCenter(body,jumpImpulse, true );
    }

    void PlayerInputSystem::onPlayerGrounded(engine::ecs::PlayerGrounded& event)
    {
        std::cout << "Player grounded";
        isPlayerGrounded = true;
    }

} // gl3