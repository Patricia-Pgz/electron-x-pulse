#pragma once
#include "engine/Game.h"

namespace gl3::engine::ecs {
    class System {
    public:
        explicit System(Game &engine) : engine(engine) {}
        System(System &&) = delete;
        System(const System &) = delete;
        virtual ~System() = default;

    protected:
        engine::Game &engine;
    };
}

