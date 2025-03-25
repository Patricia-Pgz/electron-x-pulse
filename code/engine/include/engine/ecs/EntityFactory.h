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
                                                float zRotation = 0.0f,
                                                glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                const std::string& tag = "undefined", const b2WorldId& physicsWorld)
        {
            // Create an entity
            const entt::entity entity = registry.create();

            // Add initial components
            const auto transform = registry.emplace<TransformComponent>(entity, position, zRotation, scale);
            registry.emplace<TagComponent>(entity, tag);
            if (tag != "beat")
            {
                registry.emplace<PhysicsComponent>(
                    entity, createPhysicsBody(physicsWorld, transform, entity, tag)
                );
            }
            registry.emplace<RenderComponent>(entity);
            return entity;
        }

        static PhysicsComponent createPhysicsBody(const b2WorldId& physicsWorld,
                                                  const TransformComponent& transform_component,
                                                  const entt::entity& entity,
                                                  const std::string& tag) //TODO in physicshelper oder so
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = tag == "player" ? b2_dynamicBody : b2_kinematicBody;
            bodyDef.position = {transform_component.position.x, transform_component.position.y};
            //bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
            bodyDef.fixedRotation = true;
            bodyDef.linearDamping = 0.0f;

            bodyDef.userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
            auto body = b2CreateBody(physicsWorld, &bodyDef);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 0.1f;
            shapeDef.friction = 0.0f;
            shapeDef.restitution = 0.0f;
            shapeDef.enableContactEvents = true;

            b2Polygon box = b2MakeBox(transform_component.scale.x * 0.5f, transform_component.scale.y * 0.5f);
            auto shape = b2CreatePolygonShape(body, &shapeDef, &box);
            return PhysicsComponent(physicsWorld, body, shape);
        }
    };
} // engine
