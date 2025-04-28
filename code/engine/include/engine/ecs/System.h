#pragma once
#include "engine/Game.h"

namespace gl3::engine::ecs {
    class System {
    public:
        explicit System(Game &engine) : game(engine) {}
        System(System &&) = delete;
        System(const System &) = delete;
        virtual ~System() = default;

    protected:
        Game &game;
    };
}