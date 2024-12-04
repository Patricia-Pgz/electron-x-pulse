#pragma once
#include "Entity.h"

namespace gl3 {

    class Obstacle: public Entity {
    public:
        explicit Obstacle(glm::vec3 position = glm::vec3(0, 0, 0), float size = 1, glm::vec4 color = glm::vec4(1, 0, 0, 1), b2WorldId physicsWorld = b2_nullWorldId);

    protected:
        void createPhysicsBody() override;

    public:
        void startContact() override;
    };

} // gl3

