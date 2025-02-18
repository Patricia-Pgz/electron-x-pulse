#include "Entity.h"

#include <iostream>
#include <utility>

#include "../Game.h"

namespace gl3
{
    Entity::Entity(Shader shader, Mesh mesh, glm::vec3 position, float zRotation, glm::vec3 scale, glm::vec4 color,
                   b2WorldId physicsWorld, std::string tag) :
        position(position),
        zRotation(zRotation),
        scale(scale),
        color(color),
        physicsWorld(physicsWorld),
        body(b2_nullBodyId),
        shape(b2_nullShapeId),
        shader(std::move(shader)),
        mesh(std::move(mesh)),
        initialPosition(position),
        initialZRotation(zRotation),
        initialScale(scale),
        tag(std::move(tag))
    {
    }

    Entity::~Entity()
    {
        if (b2World_IsValid(physicsWorld))
        {
            if (b2Body_IsValid(body))
                b2DestroyBody(body);
        }
    }

    void Entity::draw(Game* game)
    {
        const auto mvpMatrix = game->calculateMvpMatrix(position, zRotation, scale);
        shader.use();
        shader.setMatrix("mvp", mvpMatrix);
        shader.setVector("color", color);
        mesh.draw();
    }

    b2BodyId Entity::getBody() const
    {
        return body;
    }

    b2ShapeId Entity::getShape() const
    {
        return shape;
    }

    void Entity::updateBasedOnPhysics()
    {
        if (!b2Body_IsValid(body))
            return;

        auto physicsTransform = b2Body_GetTransform(body);

        position.x = physicsTransform.p.x;
        position.y = physicsTransform.p.y;

        zRotation = glm::degrees(b2Rot_GetAngle(physicsTransform.q));
    }

    void Entity::startContact()
    {
    }

    void Entity::resetToInitialState()
    {
        position = initialPosition;
        zRotation = initialZRotation;
        scale = initialScale;

        if (const auto body = getBody(); b2Body_IsValid(body))
        {
            b2Body_SetTransform(body, {position.x, position.y}, b2MakeRot(glm::radians(initialZRotation)));
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
            b2Body_SetAngularVelocity(body, 0.0f);
        }
    }
}
