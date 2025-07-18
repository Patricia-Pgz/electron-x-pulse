/**
* @file EventDispatcher.h
 * @brief Defines the static ECS event dispatcher for the engine.
 */

#pragma once
#include <entt/signal/dispatcher.hpp>

namespace gl3::engine::ecs
{
    /**
     * @class EventDispatcher
     * @brief Provides a global static event dispatcher for ECS systems.
     *
     * This class holds a static instance of an EnTT dispatcher,
     * allowing different systems and components to subscribe to and emit events.
     * Useful for decoupled communication within the ECS architecture.
     */
    class EventDispatcher {
    public:
        /**
         * @brief The global EnTT event dispatcher instance.
         *
         * Systems and components can publish and subscribe to events through this.
         */
        static entt::dispatcher dispatcher;
    };
}
