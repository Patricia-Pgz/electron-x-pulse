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
    static constexpr auto GROUND_SENSOR_TAG = "bottomCollider";
    static constexpr auto RIGHT_SENSOR_TAG = "rightCollider";

    ///Component for grouping objects, includes parent entity and the child's local offset to it. @note Add this to child entities
    struct ParentComponent
    {
        entt::entity parentEntity;
        glm::vec2 localOffset;
    };

    ///Parent component for grouped objects. Has a vector of child entities.
    struct GroupComponent
    {
        std::vector<entt::entity> childEntities;
    };

    ///Transform properties for entity, later used for rendering
    struct TransformComponent
    {
        explicit TransformComponent(const glm::vec3 position = {0.0f, 0.0f, 0.0f},
                                    const glm::vec3 scale = {1.0f, 1.0f, 1.0f},
                                    const float zRotation = 0.0f,
                                    const float parallax = 0.0f) :
            initialPosition(position), initialScale(scale),
            initialZRotation(zRotation), position(position), scale(scale), zRotation(zRotation),
            parallaxFactor(parallax)
        {
        }

        glm::vec3 initialPosition = {0.0f, 0.0f, 0.0f};
        glm::vec3 initialScale = {1.0f, 1.0f, 1.0f};
        float initialZRotation = 0.f;
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};
        float zRotation = 0.0f;
        float parallaxFactor = 0.f;
    };

    ///RenderComponent for entities, used in rendering system.
    struct RenderComponent
    {
        rendering::Shader shader;
        rendering::Mesh mesh;
        glm::vec4 color = {1.0f, 0.0f, 0.0f, 1.0f}; // Default red
        glm::vec4 gradientTopColor = {1, 1, 1, 1}; //can be used for gradient color
        glm::vec4 gradientBottomColor = {1, 1, 1, 1};
        const rendering::Texture* texture = nullptr;
        glm::vec4 uv;
        glm::vec2 uvOffset = {0.0f, 0.0f};
        bool isActive = true;
    };

    ///PhysicsComponent for entity. Used in physics system with Box2D body and shape.
    struct PhysicsComponent
    {
        /**
         *
         * @param physicsWorld The current Box2D world, in which the entity exists.
         * @param body The entity's Box2D physics body.
         * @param shape The entity's Box2D physics shape.
         * @param groundSensor An additional Sensor for the player to check if they are grounded.
         * @param rightSensor An additional Sensor for the player to check if they hit something from the left.
         */
        PhysicsComponent(const b2WorldId physicsWorld, const b2BodyId body, const b2ShapeId shape,
                         const b2ShapeId groundSensor = b2_nullShapeId,
                         const b2ShapeId rightSensor = b2_nullShapeId): physicsWorld(physicsWorld), body(body),
                                                                        shape(shape),
                                                                        groundSensorShape(groundSensor),
                                                                        rightWallSensorShape(rightSensor)
        {
        }

        b2WorldId physicsWorld = b2_nullWorldId;
        b2BodyId body = b2_nullBodyId;
        b2ShapeId shape = b2_nullShapeId;

        // Only used for player
        b2ShapeId groundSensorShape = b2_nullShapeId;
        b2ShapeId rightWallSensorShape = b2_nullShapeId;
        bool isActive = true;
    };

    ///TagComponent for an entity, that needs to be identified by a string tag.
    struct TagComponent
    {
        std::string tag = "undefined";
    };

    /**
     *Provides methods and helpers to create and delete enTT entity (either quad or triangle) with basic components.
     */
    class EntityFactory
    {
    public:
        static inline std::unordered_set<entt::entity> entitiesMarkedForDeletion_;

        /**
         * A wrapper for faster creation of entities with specific components (defined by @param object)
         * @param object The object from which to create components for the entity.
         * @param registry The game's enTT registry, which holds the entities.
         * @param physicsWorld The current Box2D physics world.
         * @return The newly created entity ID
         */
        static entt::entity createDefaultEntity(GameObject& object, entt::registry& registry,
                                                const b2WorldId& physicsWorld = b2_nullWorldId)
        {
            // Create an entity
            const entt::entity entity = registry.create();
            // Add initial components
            registry.emplace<TransformComponent>(
                entity, object.position, object.scale, object.rotation, object.parallaxFactor
            );
            registry.emplace<TagComponent>(entity, object.tag);
            if (object.generatePhysicsComp)
            {
                registry.emplace<PhysicsComponent>(
                    entity, createPhysicsBody(physicsWorld, entity, object)
                );
            }
            const rendering::Texture* tex = object.textureName.empty()
                                                ? nullptr
                                                : rendering::TextureManager::getTileOrSingleTex(object.textureName);
            if (object.generateRenderComp)
            {
                registry.emplace<RenderComponent>(
                    entity, createRenderComponent(object, tex));
            }

            return entity;
        };

        /**
         *Call this to empty the registry and correctly delete the components (preferably after update loop / @see static void markEntityForDeletion(entt::entity entity))
         */
        static void clearRegistry(entt::registry& registry)
        {
            for (const auto view = registry.view<PhysicsComponent>(); auto& entity : view)
            {
                destroyPhysicsComponent(registry, entity);
            }

            registry.clear();
        }

        /**
         * Destroys an entities PhysicsComponent the correct way, if it has one.
         * @param registry The current enTT registry
         * @param entity The entity of which to destroy the Physics Component
         */
        static void destroyPhysicsComponent(entt::registry& registry, const entt::entity entity)
        {
            if (!registry.valid(entity))
                return;
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


        static void markEntityForDeletion(entt::entity entity)
        {
            entitiesMarkedForDeletion_.emplace(entity);
        }

        static void deleteMarkedEntities(entt::registry& registry)
        {
            for (const entt::entity& entity : entitiesMarkedForDeletion_)
            {
                if (registry.valid(entity))
                {
                    destroyPhysicsComponent(registry, entity);
                    registry.destroy(entity);
                }
            }
            entitiesMarkedForDeletion_.clear();
        }

        ///Deletes an entity and its components, that was created with the createDefaultEntity method.
        static void deleteDefaultEntity(entt::registry& registry, const entt::entity entity)
        {
            destroyPhysicsComponent(registry, entity);
            registry.destroy(entity);
        }

        /**
         * Changes the Scale in an entities Transform and Physics Component. @not This is meant for entities that actually have both a Transform and Physics Component.
         * @param registry The current enTT registry
         * @param entity The entity that should be scales
         * @param newScale The scale to apply to the entity's Transform and PhysicsComponent.
         */
        static void setScale(entt::registry& registry, const entt::entity& entity, const glm::vec3& newScale)
        {
            const auto& tag_comp = registry.get<TagComponent>(entity);
            auto& transform = registry.get<TransformComponent>(entity);
            const auto& physics_comp = registry.get<PhysicsComponent>(entity);
            transform.scale = newScale;
            const auto polygon = createPolygon(tag_comp.tag == "obstacle", transform.scale.x, transform.scale.y);
            b2Shape_SetPolygon(physics_comp.shape, &polygon);
        };

        /**
         * Changes the position of an entities Transform and PhysicsComponent. @note This is only meant for entities with both Transform and Physics Component.
         * @param registry The current enTT registry
         * @param entity The entity of which to change position
         * @param newPos The new position for the entity's Transform and PhysicsComponent
         */
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

        /**
         * Creates a RenderComponent from properties in @param object to render an entity from in the RenderingSystem.
         * @param object The GameObject holding the properties for generating the RenderComponent
         * @param texture A pointer to a texture, is null_ptr if a color should be used instead
         * @return The newly created RenderComponent for an entity.
         */
        static RenderComponent createRenderComponent(const GameObject& object,
                                                     const rendering::Texture* texture)
        {
            auto data = object.isTriangle
                            ? getTriangleVertices(1.f, 1.f, object.uv)
                            : getBoxVertices(1.f, 1.f, object.uv);
            const std::vector<float> vertices = data.vertices;
            const std::vector<unsigned int> indices = data.indices;
            const std::string vertexPath = !object.vertexShaderPath.empty()
                                               ? object.vertexShaderPath
                                               : "shaders/vertexShader.vert";
            const std::string fragmentPath = !object.fragmentShaderPath.empty()
                                                 ? object.fragmentShaderPath
                                                 : "shaders/fragmentShader.frag";
            return RenderComponent(
                rendering::Shader(vertexPath, fragmentPath),
                rendering::Mesh(vertices, indices),
                object.color,
                object.gradientTopColor,
                object.gradientBottomColor,
                texture,
                object.uv
            );
        }

        /**
         * Creates the physics body and shape for a given enTT entity, set by properties from @param object
         * @param physicsWorld The current Box2D physics world.
         * @param entity The entity for which to create the body and shape.
         * @param object The GameObject from which to take parameters like scale.
         * @return The newly created PhysicsComponent for the entity.
         */
        static PhysicsComponent createPhysicsBody(const b2WorldId& physicsWorld,
                                                  const entt::entity& entity,
                                                  const GameObject& object)
        {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = object.tag == "player" ? b2_dynamicBody : b2_kinematicBody;
            bodyDef.position = {object.position.x, object.position.y};
            bodyDef.rotation = b2MakeRot(glm::radians(object.rotation));
            bodyDef.fixedRotation = true;
            bodyDef.linearDamping = 0.0f;
            bodyDef.userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
            const auto body = b2CreateBody(physicsWorld, &bodyDef);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 0.f;
            shapeDef.friction = 0.0f;
            shapeDef.restitution = 0.0f;

            std::vector<b2ShapeId> sensors;
            if (object.tag == "player")
            {
                shapeDef.enableContactEvents = true;

                //create additional sensors for player ground and collision checks.
                sensors = createSensors(object, body);
            }

            const b2Polygon polygon = createPolygon(object.isTriangle, object.scale.x,
                                                    object.scale.y);
            const b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &polygon);

            return object.tag == "player"
                       ? PhysicsComponent(physicsWorld, body, shape, sensors[0], sensors[1])
                       : PhysicsComponent(physicsWorld, body, shape);
        };

        /**
         *
         * @param isTriangle Determines to generate vertices for a triangle or quad.
         * @param scaleX The full scale it should have on x-axis.
         * @param scaleY Full scale on y-axis.
         * @return A b2Polygon, either quad or triangle for creating physics shapes.
         */
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

    private:
        /**
         * Creates the additional sensors for player contacts.
         * @param player The current level's player GameObject.
         * @param playerBody The player's Box2D body.
         * @return A vector with the player's ground and right sensor for ground and collision checks.
         */
        static std::vector<b2ShapeId> createSensors(const GameObject& player, const b2BodyId playerBody)
        {
            const float halfWidth = player.scale.x * 0.5f;
            const float halfHeight = player.scale.y * 0.5f;
            b2ShapeDef groundSensorDef = b2DefaultShapeDef();
            b2ShapeDef rightSensorDef = b2DefaultShapeDef();
            // ground sensor
            groundSensorDef.isSensor = true;
            groundSensorDef.enableContactEvents = true;
            groundSensorDef.userData = const_cast<void*>(static_cast<const void*>(GROUND_SENSOR_TAG));

            b2Polygon groundBox;
            groundBox = b2MakeOffsetBox(halfWidth * 0.8f, 0.15f,
                                        {0.f, 0.f - halfHeight - 0.15f},
                                        b2MakeRot(0.0f));
            b2ShapeId groundSensor = b2CreatePolygonShape(playerBody, &groundSensorDef, &groundBox);

            // Right Side Sensor
            rightSensorDef.isSensor = true;
            rightSensorDef.enableContactEvents = true;
            rightSensorDef.userData = const_cast<void*>(static_cast<const void*>(GROUND_SENSOR_TAG));

            b2Polygon rightBox;
            rightBox = b2MakeOffsetBox(0.15f, halfHeight * 0.8f,
                                       {0.f + halfWidth + 0.15f, 0.f},
                                       b2MakeRot(0.0f));
            b2ShapeId rightSensor = b2CreatePolygonShape(playerBody, &rightSensorDef, &rightBox);
            return {groundSensor, rightSensor};
        }
    };
} // engine
