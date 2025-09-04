#pragma once
#include <entt/entt.hpp>
#include "glm/vec3.hpp"
#include "../rendering/Mesh.h"
#include "../rendering/Shader.h"
#include "box2d/id.h"
#include "engine/rendering/Texture.h"
#include "engine/levelLoading/Objects.h"
#include "engine/rendering/TextureManager.h"
#include "glm/gtc/epsilon.hpp"

namespace gl3::engine::ecs
{
    /**
     * @brief Component for z-sorting during rendering
     */
    struct ZLayerComponent
    {
        float zLayer = 0.f;
    };

    /**
     * @brief Component for grouping child entities under a parent.
     *
     * Holds a reference to the parent entity and the child's local offset relative to the parent.
     * @note Add this component to child entities that belong to a group.
     */
    struct ParentComponent
    {
        entt::entity parentEntity;
        glm::vec2 localOffset;
    };

    /**
     * @brief Component for parent entities that group multiple child entities.
     *
     * Contains a vector of child entities that belong to this group.
     */
    struct GroupComponent
    {
        std::vector<entt::entity> childEntities;
    };

    /**
     * @brief Stores transform properties for an entity, used during rendering.
     *
     * Holds initial and current position, scale, rotation, and parallax factor.
     * Supports (2D & 3D) position and scale with rotation around the Z axis.
     */
    struct TransformComponent
    {
        /**
         * @brief Construct a new TransformComponent with optional initial values.
         * @param position Initial position in world space.
         * @param scale Initial scale factors.
         * @param zRotation Initial rotation around Z axis in degrees.
         * @param parallax Parallax factor for uv parallax effect.
         */
        explicit TransformComponent(const glm::vec3 position = {0.0f, 0.0f, 0.0f},
                                    const glm::vec3 scale = {1.0f, 1.0f, 1.0f},
                                    const float zRotation = 0.0f,
                                    const float parallax = 0.0f) :
            initialPosition(position), previousPosition(position), initialScale(scale),
            initialZRotation(zRotation), position(position), scale(scale), zRotation(zRotation),
            parallaxFactor(parallax)
        {
        }

        glm::vec3 initialPosition;
        glm::vec2 previousPosition;
        glm::vec3 initialScale;
        float initialZRotation;
        glm::vec3 position;
        glm::vec3 scale;
        float zRotation;
        float parallaxFactor;
    };

    /**
     * @brief Component storing rendering data for an entity.
     *
     * Includes shader, mesh, colors, texture, UV mapping, and active state.
     * Used by the rendering system to draw the entity.
     */
    struct RenderComponent
    {
        rendering::Shader shader;
        rendering::Mesh mesh;
        glm::vec4 color = {1.0f, 0.0f, 0.0f, 1.0f}; /**< Base color tint (default red), used if not using texture. */
        glm::vec4 gradientTopColor = {1, 1, 1, 1}; /**< Top color for gradient effects. */
        glm::vec4 gradientBottomColor = {1, 1, 1, 1}; /**< Bottom color for gradient effects. */
        const rendering::Texture* texture = nullptr;
        float repeatX = 0.f; /**< How often the texture is repeated on x */
        glm::vec4 uv; /**< UV coordinates for texture mapping. */
        glm::vec2 uvOffset = {0.0f, 0.0f}; /**< Offset applied to UV mapping. */
        bool isActive = true;
    };

    /**
     * @brief Component representing physics properties of an entity.
     *
     * Wraps Box2D physics objects: world, body, shapes, and optional sensors.
     * Used in the physics system to simulate physical behavior and collisions.
     */
    struct PhysicsComponent
    {
        /**
         * @brief Construct a new PhysicsComponent.
         * @param physicsWorld The Box2D physics world containing this entity.
         * @param body The Box2D body representing the entity.
         * @param shape The main Box2D shape (fixture) for collisions.
         * @param sensors Additional custom Box2D sensor shapes for custom contact event handling
         */
        PhysicsComponent(const b2WorldId physicsWorld, const b2BodyId body, const b2ShapeId shape,
                         const std::vector<b2ShapeId>& sensors = {}): physicsWorld(physicsWorld), body(body),
                                                                      shape(shape), sensorShapes(sensors)
        {
        }

        b2WorldId physicsWorld = b2_nullWorldId;
        b2BodyId body = b2_nullBodyId;
        b2ShapeId shape = b2_nullShapeId;

        // Sensor for custom sensor events
        std::vector<b2ShapeId> sensorShapes; //e.g. player has: ground, right, bottom right corner sensors
        bool isActive = true;
    };

    /**
     * @brief Component to assign a string tag to an entity.
     *
     * Used to identify or categorize entities by a human-readable tag.
     * The default tag value is "undefined".
     */
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
            // Add a layer component for render sorting
            registry.emplace<ZLayerComponent>(entity, object.zLayer);
            // Add initial components
            registry.emplace<TransformComponent>(
                entity, object.position, object.scale, object.zRotation, object.parallaxFactor
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
                auto& physicsComp = registry.get<PhysicsComponent>(entity);

                if (b2Body_IsValid(physicsComp.body))
                {
                    //destroy all shapes
                    for (b2ShapeId shape : physicsComp.sensorShapes) {
                        if (b2Shape_IsValid(shape)) {
                            b2DestroyShape(shape, false);
                        }
                    }
                    physicsComp.sensorShapes.clear();
                    if (b2Shape_IsValid(physicsComp.shape)) {
                        b2DestroyShape(physicsComp.shape, false);
                    }

                    //destroy body
                    b2DestroyBody(physicsComp.body);
                    physicsComp.body = b2_nullBodyId;
                }
            }
            registry.remove<PhysicsComponent>(entity);
        }

        /**
         * @brief Mark an entity to be deleted later.
         *
         * Adds the given entity to a static deletion queue.
         * Use this to safely defer deletion until later.
         *
         * @param entity The entity to mark for deletion.
         */
        static void markEntityForDeletion(entt::entity entity)
        {
            entitiesMarkedForDeletion_.emplace(entity);
        }

        /**
         * @brief Delete all entities previously marked for deletion.
         *
         * Iterates over the deletion queue, destroys any valid entities,
         * and removes their physics components if present.
         * Clears the deletion queue afterward.
         *
         * @param registry The EnTT registry containing the entities.
         */
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

        /**
         * @brief Immediately delete an entity and its components.
         *
         * Destroys an entity created with createDefaultEntity, including
         * removing its physics component if it has one.
         *
         * @param registry The EnTT registry containing the entity.
         * @param entity The entity to delete.
         */
        static void deleteDefaultEntity(entt::registry& registry, const entt::entity entity)
        {
            destroyPhysicsComponent(registry, entity);
            registry.destroy(entity);
        }

        /**
         * Changes the Scale in an entities Transform and Physics Component. @note This is meant for entities that actually have both a Transform and Physics Component.
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
            const auto& physics_comp = registry.get<PhysicsComponent>(entity);
            transform.position = newPos;
            b2Body_SetTransform(physics_comp.body, b2Vec2(transform.position.x, transform.position.y),
                                b2Body_GetRotation(physics_comp.body));
        }

        /**
         * Changes the z rotation of an entities Transform and PhysicsComponent. @note This is only meant for entities with both Transform and Physics Component.
         * @param registry The current enTT registry
         * @param entity The entity to rotate
         * @param newZRot The new rotation to apply on z
         */
        static void SetRotation(entt::registry& registry, const entt::entity& entity, const float newZRot)
        {
            auto& transform = registry.get<TransformComponent>(entity);
            const auto& physics_comp = registry.get<PhysicsComponent>(entity);
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

        static glData getBoxVertices(const float width, const float height, glm::vec4 uv, const float repeatX)
        {
            glData boxData;

            float leftU = uv.x;
            float rightU = repeatX > 0.f ? uv.x + repeatX : uv.z;

            boxData.vertices = {
                -width / 2, height / 2, 0.0f, leftU, uv.w,
                -width / 2, -height / 2, 0.0f, leftU, uv.y,
                width / 2, -height / 2, 0.0f, rightU, uv.y,
                width / 2, height / 2, 0.0f, rightU, uv.w
            };
            boxData.indices = {
                0, 1, 2,
                0, 2, 3
            };
            return boxData;
        }

        /**
         * Creates a RenderComponent from properties in @param object to render an entity from in the RenderingSystem.
         * @param object The GameObject holding the properties for generating the RenderComponent
         * @param texture A pointer to a texture, is null_ptr if a color should be used instead
         * @return The newly created RenderComponent for an entity.
         */
        static RenderComponent createRenderComponent(GameObject& object,
                                                     const rendering::Texture* texture)
        {
            float repeatX = 1.f;
            if (texture)
            {
                if (!texture->isTileSet() && object.repeatTextureX)
                {
                    //repeat texture on x to keep textures aspect ratio
                    const float texAspect = static_cast<float>(texture->getWidth()) / static_cast<float>(texture->getHeight());
                    repeatX = object.scale.x / (object.scale.y * texAspect);
                } else
                {
                    repeatX = 0.f;
                }
            }
            //use gradient shader
            if (!all(epsilonEqual(object.gradientTopColor, object.gradientBottomColor, 0.001f)))
            {
                object.vertexShaderPath = "shaders/gradient.vert";
                object.fragmentShaderPath = "shaders/gradient.frag";
            }
            const auto data = object.isTriangle
                                  ? getTriangleVertices(1.f, 1.f, object.uv)
                                  : getBoxVertices(1.f, 1.f, object.uv, repeatX);
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
                repeatX,
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
            bodyDef.rotation = b2MakeRot(glm::radians(object.zRotation));
            bodyDef.fixedRotation = object.tag == "player";
            bodyDef.isBullet = object.tag == "player"; //stop tunneling / continuously update collision detection
            bodyDef.linearDamping = 0.0f;
            bodyDef.userData = reinterpret_cast<void*>(static_cast<uintptr_t>(entity));
            const auto body = b2CreateBody(physicsWorld, &bodyDef);

            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 0.f;
            shapeDef.friction = 0.0f;
            shapeDef.restitution = 0.0f;
            shapeDef.isSensor = object.isSensor;

            std::vector<b2ShapeId> sensors;
            if (object.tag == "player")
            {
                //create additional sensors for player ground and collision checks.
                sensors = createSensors(object, body);
            }

            const b2Polygon polygon = createPolygon(object.isTriangle, object.scale.x,
                                                    object.scale.y);
            const b2ShapeId shape = b2CreatePolygonShape(body, &shapeDef, &polygon);

            return object.tag == "player"
                       ? PhysicsComponent(physicsWorld, body, shape, sensors)
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
            b2ShapeDef topSensorDef = b2DefaultShapeDef();

            // ground sensor
            groundSensorDef.isSensor = true;
            groundSensorDef.enableContactEvents = true;

            b2Polygon groundBox;
            groundBox = b2MakeOffsetBox(halfWidth * 0.8f, 0.05f,
                                        {0.f, 0.f - halfHeight + 0.05f},
                                        b2MakeRot(0.0f));
            b2ShapeId groundSensor = b2CreatePolygonShape(playerBody, &groundSensorDef, &groundBox);

            // ground sensor
            topSensorDef.isSensor = true;
            topSensorDef.enableContactEvents = true;

            b2Polygon topBox;
            topBox = b2MakeOffsetBox(halfWidth * 0.8f, 0.05f,
                                        {0.f, 0.f + halfHeight - 0.05f},
                                        b2MakeRot(0.0f));
            b2ShapeId topSensor = b2CreatePolygonShape(playerBody, &topSensorDef, &topBox);

            // Right Side collider
            rightSensorDef.enableContactEvents = true;

            b2Polygon rightBox;
            rightBox = b2MakeOffsetBox(0.05f, halfHeight * 0.5f,
                                       {0.f + halfWidth - 0.05f, 0.f},
                                       b2MakeRot(0.0f));

            b2ShapeId rightSensor = b2CreatePolygonShape(playerBody, &rightSensorDef, &rightBox);

            //for collision with corners of objects
            return {groundSensor, rightSensor, topSensor};
        }
    };
} // engine
