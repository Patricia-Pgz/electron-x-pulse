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

    /// provides methods to generate a basic entity(either quad or triangle) with basic components
    class EntityFactory
    {
    public:
        //TODO: color, scale, rotation nicht im constructor sondern über setter setzen lassen. Color wird wahrscheinlich später zu einem Sprite oder so
        static entt::entity createDefaultEntity(entt::registry& registry,
                                                const glm::vec3& position = {0.0f, 0.0f, 0.0f},
                                                const glm::vec3& scale = {1.0f, 1.0f, 1.0f},
                                                const float& zRotation = 0.0f,
                                                const glm::vec4& color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                const std::string& tag = "undefined", const b2WorldId& physicsWorld,
                                                const bool& isTriangle = false)
        {
            // Create an entity
            const entt::entity entity = registry.create();

            // Add initial components
            const auto transform = registry.emplace<TransformComponent>(entity, position, zRotation, scale);
            registry.emplace<TagComponent>(entity, tag);
            if (tag != "beat")
            {
                registry.emplace<PhysicsComponent>(
                    entity, createPhysicsBody(physicsWorld, transform, entity, tag, isTriangle)
                );
            }
            registry.emplace<RenderComponent>(entity, createRenderComponent(color, isTriangle, scale));

            return entity;
        }

        struct glData
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
        };

        static glData getTriangleVertices(const float& width, const float& height)
        {
            glData triangleData;

            // Triangle vertices (x, y, z)
            triangleData.vertices = {
                -width / 2, -height / 2, 0.0f, // Bottom-left
                width / 2, -height / 2, 0.0f, // Bottom-right
                0.0f, height / 2, 0.0f // Top-center
            };

            // Indices for one triangle
            triangleData.indices = {
                0, 1, 2 // The triangle
            };

            return triangleData;
        }

        static glData getBoxVertices(const float& width, const float& height)
        {
            glData boxData;

            // Rectangle vertices (x, y, z)
            boxData.vertices = {
                -width / 2, height / 2, 0.0f, // Top-left
                -width / 2, -height / 2, 0.0f, // Bottom-left
                width / 2, -height / 2, 0.0f, // Bottom-right
                width / 2, height / 2, 0.0f // Top-right
            };

            // Indices for two triangles
            boxData.indices = {
                0, 1, 2, // First triangle
                0, 2, 3 // Second triangle
            };

            return boxData;
        }

        static RenderComponent createRenderComponent(const glm::vec4& color, const bool& isTriangle,
                                                     const glm::vec3& scale)
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            if (isTriangle)
            {
                const glData triangleData = getTriangleVertices(scale.x, scale.y);
                vertices = triangleData.vertices;
                indices = triangleData.indices;
            }
            else
            {
                const glData boxData = getBoxVertices(scale.x, scale.y);
                vertices = boxData.vertices;
                indices = boxData.indices;
            }

            return RenderComponent(rendering::Shader{"shaders/vertexShader.vert", "shaders/fragmentShader.frag"},
                                   rendering::Mesh{vertices, indices}, color);
        }

        static PhysicsComponent createPhysicsBody(const b2WorldId& physicsWorld,
                                                  const TransformComponent& transform_component,
                                                  const entt::entity& entity,
                                                  const std::string& tag,
                                                  const bool& isTriangle = false) //TODO in physicshelper oder so
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

            b2ShapeId shape;
            if (isTriangle)
            {
                const b2Vec2 vertices[] = {
                    b2Vec2(-transform_component.scale.x * 0.5f, -transform_component.scale.y * 0.5f), // Bottom-left
                    b2Vec2(transform_component.scale.x * 0.5f, -transform_component.scale.y * 0.5f), // Bottom-right
                    b2Vec2(0.0f, transform_component.scale.y * 0.5f)
                }; // Top-center

                const b2Hull hull = b2ComputeHull(vertices, 3);

                const b2Polygon triangle = b2MakePolygon(&hull, 0.1f); // Create a polygon shape with the 3 vertices
                shape = b2CreatePolygonShape(body, &shapeDef, &triangle);
            }
            else
            {
                const b2Polygon box = b2MakeBox(transform_component.scale.x * 0.5f, transform_component.scale.y * 0.5f);
                shape = b2CreatePolygonShape(body, &shapeDef, &box);
            }

            return PhysicsComponent(physicsWorld, body, shape);
        }
    };
} // engine
