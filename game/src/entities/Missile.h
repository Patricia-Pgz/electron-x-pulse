#pragma once

#include "Entity.h"

namespace gl3 {
    class Missile: public Entity {
    public:
        explicit Missile(Game *game);
        void update(Game *game, float deltaTime) override;

    private:
        float speed = 2.0f;
    };
}



