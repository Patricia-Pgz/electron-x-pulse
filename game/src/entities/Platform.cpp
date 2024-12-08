#include "Platform.h"
#include <iostream>
#include "box2d/box2d.h"

namespace gl3 {
    struct glData {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
    };

    glData getBoxVertices(float width, float height) {
        glData boxData;

        // Rectangle vertices (x, y, z)
        boxData.vertices = {
            -width / 2,  height / 2, 0.0f,  // Top-left
            -width / 2, -height / 2, 0.0f,  // Bottom-left
             width / 2, -height / 2, 0.0f,  // Bottom-right
             width / 2,  height / 2, 0.0f   // Top-right
        };

        // Indices for two triangles
        boxData.indices = {
            0, 1, 2,  // First triangle
            0, 2, 3   // Second triangle
        };

        return boxData;
    }

    const glData glBoxData = getBoxVertices(1.0f, 1.0f);

    Platform::Platform(glm::vec3 position,  float width, float height, glm::vec4 color, b2WorldId physicsWorld)
    : Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
             Mesh(glBoxData.vertices, glBoxData.indices),
             position,
             0,
             glm::vec3(width, height, 0.0f),
             color,
             physicsWorld,
             "platform")
    {
        Platform::createPhysicsBody();
    }

    void Platform::createPhysicsBody()
    {

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_kinematicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        //shapeDef.density = 1.0f;
        shapeDef.friction = 0.0f;
        shapeDef.restitution = 0.0f;

        b2Polygon box = b2MakeBox(scale.x * 0.5f, scale.y * 0.5f);
        shape = b2CreatePolygonShape(body, &shapeDef, &box);
    }

    void Platform::startContact() {
        std::cout << "Platform start contact" << std::endl;
    }

} // gl3