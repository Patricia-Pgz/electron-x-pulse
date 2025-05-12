#pragma once
#include <entt/entity/entity.hpp>

namespace gl3::engine::ecs
{
    struct PlayerGrounded {
        entt::entity player;
    };
    struct PlayerDeath
    {
        entt::entity player;
    };
}
