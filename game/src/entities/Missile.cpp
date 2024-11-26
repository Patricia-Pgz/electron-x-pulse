#include "Missile.h"

#include <iostream>

#include "Entity.h"
#include "../Game.h"

namespace gl3 {
    Missile::Missile(gl3::Game *game, glm::vec3 position, float zRotation, float size) : Entity(
            Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            Mesh({0.0f, 0.0f, 0.0f,
                  0.0f, 0.5f, 0.0f,
                  0.2f, 0.4f, 0.0f,
                  0.2, -0.2, 0.0f,
                  0.3f, -0.3f, 0.0f,
                  0.3f, -0.4f, 0.0f,

                  -0.3f, -0.4f, 0.0f,
                  -0.3f, -0.3f, 0.0f,
                  -0.2, -0.2, 0.0f,
                  -0.2f, 0.4f, 0.0f},
                 {0, 1, 2,
                  0, 2, 3,
                  0, 3, 4,
                  0, 4, 5,
                  0, 5, 6,
                  0, 6, 7,
                  0, 7, 8,
                  0, 8, 9,
                  0, 9, 1}),
            position,
            zRotation,
            glm::vec3(size),
            {1.0f, 1.0f, 1.0f, 1.0f},
            game->getPhysicsWorld()
    )
    {
        Missile::createPhysicsBody();
    }

    void Missile::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
        bodyDef.fixedRotation = true;
        bodyDef.isBullet = true;

        // + 90 because mesh up is +y
        b2Vec2 forwardVec = {glm::cos(glm::radians(zRotation + 90)), glm::sin(glm::radians(zRotation + 90))};
        bodyDef.linearVelocity = forwardVec * speed;

        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        shapeDef.friction = 0.0f;
        shapeDef.restitution = 0.1f;

        // approximation of polygon shape
        b2Vec2 vertices[] =
        {
            {0.0f, 0.5f},
            {0.2f, 0.4f},
            {0.3f, -0.4f},
            {-0.3, -0.4},
            {-0.2f, 0.4f},
        };

        for (auto &point : vertices) {
            point.x *= scale.x;
            point.y *= scale.y;
        }

        b2Hull hull = b2ComputeHull(vertices, 5);
        b2Polygon polygon = b2MakePolygon(&hull, 0.1f);
        shape = b2CreatePolygonShape(body, &shapeDef, &polygon);
    }
    void Missile::startContact() {
        std::cout << "Missile start contact" << std::endl;
    }


}
