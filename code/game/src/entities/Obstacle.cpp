#include "Obstacle.h"
#include <iostream>
#include "box2d/box2d.h"

namespace gl3
{
    struct glData {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
    };

    glData getTriangleVertices(float width, float height) {
        glData triangleData;

        // Triangle vertices (x, y, z)
        triangleData.vertices = {
            -width / 2, -height / 2, 0.0f,  // Bottom-left
             width / 2, -height / 2, 0.0f,  // Bottom-right
             0.0f,      height / 2, 0.0f   // Top-center
        };

        // Indices for one triangle
        triangleData.indices = {
            0, 1, 2  // The triangle
        };

        return triangleData;
    }


    // Use a fixed box size (can be parameterized)
    const glData glBoxData = getTriangleVertices(1.f, 1.f);

    Obstacle::Obstacle(glm::vec3 position, float size, glm::vec4 color, b2WorldId physicsWorld)
    : Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
             Mesh(glBoxData.vertices, glBoxData.indices),
             position,
             0,
             glm::vec3(size, size, 0.0f),
             color,
             physicsWorld,
             "obstacle")
    {
        Obstacle::createPhysicsBody();
    }

    void Obstacle::createPhysicsBody()
    {

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_kinematicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        bodyDef.linearDamping = 0.0f;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.friction = 0.0f;
        //shapeDef.density = 1.0f;
        //shapeDef.friction = 0.01f;
        shapeDef.restitution = 0.0f;

        b2Vec2 vertices[] = {
            b2Vec2(-scale.x * 0.5f, -scale.y * 0.5f), // Bottom-left
            b2Vec2(scale.x * 0.5f, -scale.y * 0.5f),  // Bottom-right
                   b2Vec2( 0.0f,scale.y * 0.5f)};  // Top-center

        b2Hull hull = b2ComputeHull(vertices, 3);

        b2Polygon triangle = b2MakePolygon(&hull, 0.1f); // Create a polygon shape with the 3 vertices
        shape = b2CreatePolygonShape(body, &shapeDef, &triangle);
    }

    void Obstacle::startContact() {
        std::cout << "Obstacle start contact" << std::endl;
    }

} // gl3