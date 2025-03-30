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

        bool canJump = false;
        if (!m_jumping && velocity.y < 0.01f)
        {
            // Checking for contact with the ground or other surfaces
            int capacity = b2Body_GetContactCapacity(body);
            capacity = b2MinInt(capacity, 4); // max of 4 contacts for simplicity
            b2ContactData contactData[4];
            int count = b2Body_GetContactData(body, contactData, capacity);

            for (int i = 0; i < count; ++i)
            {
                b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
                b2BodyId bodyIdB = b2Shape_GetBody(contactData[i].shapeIdB);
                float sign = (B2_ID_EQUALS(bodyIdA, body)) ? -1.0f : 1.0f;
                auto* entityA = static_cast<entt::entity*>(b2Body_GetUserData(bodyIdA)); //TODO das hier funktioniert nicht -> collisionsystem + listener
                auto* entityB = static_cast<entt::entity*>(b2Body_GetUserData(bodyIdB));
                if (!entityA || !entityB) return;
                auto tagA = game.getRegistry().get<engine::ecs::TagComponent>(*entityA).tag;
                auto tagB = game.getRegistry().get<engine::ecs::TagComponent>(*entityB).tag;

                if (tagA == "obstacle" || tagB == "obstacle")
                {
                    std::cout << "Hit Obstacle, reset to start"; //TODO put logic in ContactListener!
                    //onPlayerDeath.invoke(); //TODO event in ContactListener
                }
                else if (sign * contactData[i].manifold.normal.y > 0.9f)
                {
                    // If the normal of the contact is nearly vertical and pointing upwards, we can jump
                    canJump = true;
                }
                else if (contactData[i].manifold.normal.x > 0.9f)
                {
                    // Almost horizontal collision to the left
                    // Player is touching from the left
                    std::cout << "Destroy Player, reset to start";
                    //onPlayerDeath.invoke(); //TODO
                }
            }
        }

        if (canJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            //Jump
            //b2Body_ApplyLinearImpulseToCenter(body, b2Vec2{0.0f, m_jumpImpulse}, true);
            applyJumpImpulse(body);
            m_jumping = true;
        }
    }


    void PlayerInputSystem::applyJumpImpulse(const b2BodyId body) const
    {
        const b2Vec2 jumpImpulse = calculateJumpImpulse(body, JumpConfig(9.81, game.getCurrentBPM(), distancePerBeat));

        b2Body_ApplyLinearImpulseToCenter(body,jumpImpulse, true );
    }

} // gl3