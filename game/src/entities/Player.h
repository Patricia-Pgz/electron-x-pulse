#pragma once

#include "Entity.h"
#include <soloud.h>
#include <soloud_wav.h>

namespace gl3 {
    class Player: public Entity {
    public:
        explicit Player(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
             float zRotation = 0.0f,
             glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f), b2WorldId physicsWorld = b2_nullWorldId);
        std::function<void()> onDestroyed;
        void setOnDestroyedCallback(std::function<void()> callback) {
            onDestroyed = std::move(callback);
        }
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void updateBasedOnPhysics() override;
        void startContact() override;

    protected:
        void createPhysicsBody() override;

    private:
        void applyJumpImpulse(Game* game);
        SoLoud::Soloud audio;
        SoLoud::Wav firingSound;
        bool m_jumping =false;
        bool m_canJump = true;
        float m_jumpImpulse = 0.4f;
        float translationSpeed = 3.0f;
        float rotationSpeed = 360.0f;
        const float timeBetweenShots = 0.1;
        float countdownUntilNextShot = timeBetweenShots;
    };
}


