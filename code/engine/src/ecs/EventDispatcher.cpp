/**
* @file EventDispatcher.cpp
 * @brief Defines the static dispatcher instance for the EventDispatcher class.
 */
#include "engine/ecs/EventDispatcher.h"

namespace gl3::engine::ecs {
    /**
     * @brief Definition of the static dispatcher used for ECS event handling.
     */
    entt::dispatcher EventDispatcher::dispatcher;

} // namespace gl3::engine::ecs
