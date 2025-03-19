#pragma once
#include <entt/entt.hpp>
#include "glm/vec3.hpp"
#include "../rendering/Mesh.h"
#include "../rendering/Shader.h"
#include "box2d/id.h"

namespace gl3::engine::ecs
{
    struct TransformComponent
    {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        float zRotation = 0.0f;
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    };

    struct RenderComponent
    {
        rendering::Shader shader;
        rendering::Mesh mesh;
        glm::vec4 color = {1.0f, 0.0f, 0.0f, 1.0f}; // Default red
    };

    struct PhysicsComponent
    {
        b2WorldId physicsWorld = b2_nullWorldId;
        b2BodyId body = b2_nullBodyId;
        b2ShapeId shape = b2_nullShapeId;
    };

    struct TagComponent
    {
        std::string tag = "undefined";
    };

    /// provides methods to generate a basic entity with basic components
    class EntityFactory
    {
    public:
        static entt::entity createDefaultEntity(entt::registry& registry, glm::vec3 position = {0.0f, 0.0f, 0.0f},
                                           glm::vec3 scale = {1.0f, 1.0f, 1.0f},
                                           float zRotation = 0.0f, glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), const std::string& tag = "undefined")
        {
            // Create an entity
            const entt::entity entity = registry.create();

            // Add initial components
            registry.emplace<TransformComponent>(entity, position, scale, zRotation);
            registry.emplace<TagComponent>(entity, tag);
            registry.emplace<PhysicsComponent>(entity);
            registry.emplace<RenderComponent>(entity);
            return entity;
        }
};

} // engine
