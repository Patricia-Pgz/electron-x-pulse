#pragma once
#include <entt/entt.hpp>
#include "glm/vec3.hpp"
#include "../rendering/Mesh.h"
#include "../rendering/Shader.h"
#include "box2d/id.h"
#include "engine/rendering/Texture.h"
#include "engine/levelLoading/Objects.h"
#include "engine/rendering/TextureManager.h"

namespace gl3::engine::ecs
{
    struct TransformComponent
    {
        TransformComponent(const glm::vec3 position, const glm::vec3 scale, const float zRotation) :
            initialPosition(position), initialScale(scale),
            initialZRotation(zRotation), position(position), scale(scale), zRotation(zRotation)
        {
        }

        glm::vec3 initialPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 initialScale = {0.f, 0.f, 0.f};
        float initialZRotation = 0.f;
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};
        float zRotation = 0.0f;
    };

    struct RenderComponent
    {
        rendering::Shader shader;
        rendering::Mesh mesh;
        glm::vec4 color = {1.0f, 0.0f, 0.0f, 1.0f}; // Default red
        const rendering::Texture* texture = nullptr;
        glm::vec4 uv;
        bool isActive = true;
    };

    struct PhysicsComponent
    {
        b2WorldId physicsWorld = b2_nullWorldId;
        b2BodyId body = b2_nullBodyId;
        b2ShapeId shape = b2_nullShapeId;
        bool isActive = true;
    };

    struct TagComponent
    {
        std::string tag = "undefined";
    };

    /// provides methods to generate a basic entity (either quad or triangle) with basic components
    class EntityFactory
    {
    public:
        static entt::entity createDefaultEntity(GameObject& object, entt::registry& registry,
                                                const b2WorldId& physicsWorld = b2_nullWorldId)
        {
            // Create an entity
            const entt::entity entity = registry.create();
            // Add initial components
            const auto transform = registry.emplace<TransformComponent>(
                entity, object.position, object.scale, object.rotation);
            registry.emplace<TagComponent>(entity, object.tag);
            if (object.enableCollision)
            {
                registry.emplace<PhysicsComponent>(
                    entity, createPhysicsBody(physicsWorld, transform, entity, object.tag, object.isTriangle)
                );
            }
            const rendering::Texture* tex = object.textureName.empty()
                                                ? nullptr
                                                : rendering::TextureManager::get(object.textureName);
            registry.emplace<RenderComponent>(
                entity, createRenderComponent(object.color, object.isTriangle, tex, object.uv));

            return entity;
        };

        /*
         *Call this to empty the registry and correctly delete the components
         */
        static void clearRegistry(entt::registry& registry)
        {
            for (const auto view = registry.view<PhysicsComponent>(); auto& entity : view)
            {
                destroyPhysicsComponent(registry, entity);
            }

            registry.clear();
        }

        static void destroyPhysicsComponent(entt::registry& registry, const entt::entity entity)
        {
            if (registry.all_of<PhysicsComponent>(entity))
            {
                auto& physics = registry.get<PhysicsComponent>(entity);

                if (b2Body_IsValid(physics.body))
                {
                    if (b2Shape_IsValid(physics.shape))
                    {
                        b2DestroyShape(physics.shape, false);
                        physics.shape = b2_nullShapeId;
                    }
                    b2DestroyBody(physics.body);
                    physics.body = b2_nullBodyId;
                }
            }
            registry.remove<PhysicsComponent>(entity);
        }


        ///Deletes an entity and its components, that was created with the createDefaultEntity method.
        static void deleteDefaultEntity(entt::registry& registry, const entt::entity entity)
        {
            destroyPhysicsComponent(registry, entity);
            registry.destroy(entity);
        }

        static void setScale(entt::registry& registry, const entt::entity& entity, const glm::vec3& newScale)
        {
            const auto& tag_comp = registry.get<TagComponent>(entity);
            auto& transform = registry.get<TransformComponent>(entity);
            const auto& physics_comp = registry.get<PhysicsComponent>(entity);
            transform.scale = newScale;
            const auto polygon = createPolygon(tag_comp.tag == "obstacle", transform.scale.x, transform.scale.y);
            b2Shape_SetPolygon(physics_comp.shape, &polygon);
        };

        static void setPosition(entt::registry& registry, const entt::entity& entity, const glm::vec3& newPos)
        {
            auto& transform = registry.get<TransformComponent>(entity);
            auto& physics_comp = registry.get<PhysicsComponent>(entity);
            transform.position = newPos;
            b2Body_SetTransform(physics_comp.body, b2Vec2(transform.position.x, transform.position.y),
                                b2Body_GetRotation(physics_comp.body));
        }

        static void SetRotation(entt::registry& registry, const entt::entity& entity, const float newZRot)
        {
            auto& transform = registry.get<TransformComponent>(entity);
            auto& physics_comp = registry.get<PhysicsComponent>(entity);
            transform.zRotation = newZRot;
            b2Body_SetTransform(physics_comp.body, b2Body_GetPosition(physics_comp.body),
                                b2MakeRot(glm::radians(newZRot)));
        }

        struct glData
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
        };

        static glData getTriangleVertices(const float& width, const float& height, const glm::vec4& uv)
        {
            glData triangleData;

            float uMid = (uv.x + uv.z) * 0.5f;
            float vTop = uv.w;
            float vBottom = uv.y;

            // Triangle vertices (x, y, z)
            triangleData.vertices = {
                -width / 2, -height / 2, 0.0f, uv.x, vBottom, // Bottom-left
                width / 2, -height / 2, 0.0f, uv.z, vBottom, // Bottom-right

                0.0f, height / 2, 0.0f, uMid, vTop // Top-center
            };

            // Indices for one triangle
            triangleData.indices = {
                0, 1, 2 // The triangle
            };

            return triangleData;
        }

        static glData getBoxVertices(const float& width, const float& height, const glm::vec4& uv)
        {
            glData boxData;

            // Rectangle vertices (x, y, z)
            boxData.vertices = {
                -width / 2, height / 2, 0.0f, uv.x, uv.w, // Top-left
                -width / 2, -height / 2, 0.0f, uv.x, uv.y, // Bottom-left
                width / 2, -height / 2, 0.0f, uv.z, uv.y, // Bottom-right
                width / 2, height / 2, 0.0f, uv.z, uv.w // Top-right
            };

            // Indices for two triangles
            boxData.indices = {
                0, 1, 2, // First triangle
                0, 2, 3 // Second triangle
            };

            return boxData;
        }

        static RenderComponent createRenderComponent(const glm::vec4& color, const bool& isTriangle,
                                                     const rendering::Texture* texture, const glm::vec4& uv)
        {
            auto data = isTriangle ? getTriangleVertices(1.f, 1.f, uv) : getBoxVertices(1.f, 1.f, uv);
            const std::vector<float> vertices = data.vertices;
            const std::vector<unsigned int> indices = data.indices;

            return RenderComponent(
                rendering::Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
                rendering::Mesh(vertices, indices),
                color,
                texture
            );
        }

        static PhysicsComponent createPhysicsBody(const b2WorldId& physicsWorld,
                                                  const TransformComponent& transform_component,
                                                  const entt::entity& entity,
                                                  const std::string& tag,
                                                  const bool isTriangle = false)
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = tag == "player" ? b2_dynamicBody : b2_kinematicBody;
            bodyDef.position = {transform_component.position.x, transform_component.position.y};
            //bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
            bodyDef.fixedRotation = true;
            bodyDef.linearDamping = 0.0f;
            bodyDef.userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
            const auto body = b2CreateBody(physicsWorld, &bodyDef);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 0.1f;
            shapeDef.friction = 0.0f;
            shapeDef.restitution = 0.0f;
            if (tag == "player")
            {
                shapeDef.enableContactEvents = true;
            }

            const b2Polygon polygon = createPolygon(isTriangle, transform_component.scale.x,
                                                    transform_component.scale.y);
            const b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &polygon);

            return PhysicsComponent(physicsWorld, body, shape);
        };

        static b2Polygon createPolygon(const bool isTriangle, const float scaleX, const float scaleY)
        {
            b2Polygon polygon;
            if (isTriangle)
            {
                const b2Vec2 vertices[] = {
                    b2Vec2(-scaleX * 0.5f, -scaleY * 0.5f), // Bottom-left
                    b2Vec2(scaleX * 0.5f, -scaleY * 0.5f), // Bottom-right
                    b2Vec2(0.0f, scaleY * 0.5f) // Top-center
                };

                const b2Hull hull = b2ComputeHull(vertices, 3);
                polygon = b2MakePolygon(&hull, 0.1f);
            }
            else
            {
                polygon = b2MakeBox(scaleX * 0.5f, scaleY * 0.5f);
            }

            return polygon;
        }
    };
} // engine
