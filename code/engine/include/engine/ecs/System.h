#pragma once
#include "engine/Game.h"

namespace gl3::engine::ecs {
    class System {
    public:
        explicit System(Game &engine) : game_(engine) {}
        System(System &&) = delete;
        System(const System &) = delete;
        virtual ~System() = default;
        void setActive(const bool setActive)
        {
            is_active = setActive;
        }

    protected:
        Game &game_;
        bool is_active = true;
    };
}