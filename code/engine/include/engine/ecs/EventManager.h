#pragma once
#include <entt/signal/dispatcher.hpp>

namespace gl3::engine::ecs
{
    class EventManager {
    public:
        static entt::dispatcher dispatcher;
    };
}
