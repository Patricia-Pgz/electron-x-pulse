#pragma once
#include <entt/entity/registry.hpp>

#include "ecs/EntityFactory.h"

namespace gl3::engine::visual_effects
{
    class Parallax
    {
    public:
        static void moveBgObjectsParallax(entt::registry& registry, const float deltaTime, const float levelSpeed)
        {
            if(levelSpeed == 0) return;
            const auto& entities = registry.view<
                ecs::TagComponent, ecs::TransformComponent>();
            for (auto& entity : entities)
            {
                if (entities.get<ecs::TagComponent>(entity).tag != "background")return;
                auto& transform = entities.get<ecs::TransformComponent>(entity);
                transform.position.x -= levelSpeed * deltaTime * transform.parallaxFactor;
            }
        }
    };
}
