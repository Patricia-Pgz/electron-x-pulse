#include "Ship.h"

#include <iostream>

#include "../Game.h"
#include "../Assets.h"
#include <cmath>

#include "Obstacle.h"


namespace gl3
{
    Ship::Ship(glm::vec3 position, float zRotation, glm::vec3 scale, b2WorldId physicsWorld) : Entity(
        Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
        Mesh({
                 -0.5f, 0.5f, 0.0f,
                 -0.5f, -0.5f, 0.0f,
                 0.5f, 0.5f, 0.0f,
                 0.5f, -0.5f, 0.0f
             },
             {
                 0, 1, 2,
                 1, 2, 3
             }),
        position,
        zRotation,
        scale,
        {1, 1, 1, 1},
        physicsWorld,
        "player")
    {
        audio.init();
        audio.setGlobalVolume(0.1f);
        firingSound.load(resolveAssetPath("audio/shot.mp3").string().c_str());
        firingSound.setSingleInstance(true);
        Ship::createPhysicsBody();
    }

    void Ship::update(Game* game, float deltaTime)
    {
        auto window = game->getWindow();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        auto body = getBody();
        b2Vec2 velocity = b2Body_GetLinearVelocity(body);

        // Check if we are allowed to jump (not jumping already, and on the ground)
        bool canJump = false;
        if (!m_jumping && velocity.y < 0.01f)
        {
            // Checking for contact with the ground or other surfaces
            int capacity = b2Body_GetContactCapacity(body);
            capacity = b2MinInt(capacity, 4); // max of 4 contacts for simplicity
            b2ContactData contactData[4];
            int count = b2Body_GetContactData(body, contactData, capacity);

            for (int i = 0; i < count; ++i)
            {
                b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
                b2BodyId bodyIdB = b2Shape_GetBody(contactData[i].shapeIdB);
                float sign = (B2_ID_EQUALS(bodyIdA, body)) ? -1.0f : 1.0f;
                auto entityA = (Entity*)(b2Body_GetUserData(bodyIdA));
                auto entityB = (Entity*)(b2Body_GetUserData(bodyIdB));
                if (entityA == nullptr || entityB == nullptr)
                {
                    std::cout << "Collision with null";
                }
                else if (entityA->getTag() == "obstacle" || entityB->getTag() == "obstacle")
                {
                    std::cout << "Hit Obstacle, reset to start"; //TODO put logic in start contact?
                    onDestroyed();
                }
                else if (sign * contactData[i].manifold.normal.y > 0.9f)
                {
                    // If the normal of the contact is nearly vertical and pointing upwards, we can jump
                    canJump = true;
                }
                else if (contactData[i].manifold.normal.x > 0.9f)
                {
                    // Almost horizontal collision to the left
                    // Player is touching from the left
                    std::cout << "Destroy Player, reset to start"; //TODO put logic in start contact?
                    onDestroyed();
                }
            }
        }

        if (canJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            //Jump
            b2Body_ApplyLinearImpulseToCenter(body, b2Vec2{0.0f, m_jumpImpulse}, true);
            m_jumping = true;
        }
    }

    void Ship::draw(Game* game)
    {
        Entity::draw(game);
    }

    void Ship::createPhysicsBody()
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        //bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
        bodyDef.fixedRotation = true;
        //bodyDef.linearDamping = 0.5f;

        bodyDef.userData = static_cast<void*>(this);
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.friction = 0.0f;
        shapeDef.restitution = 0.0f;
        shapeDef.enableContactEvents = true;

        b2Polygon box = b2MakeBox(scale.x * 0.5f, scale.y * 0.5f);
        shape = b2CreatePolygonShape(body, &shapeDef, &box);
    }

    void Ship::updateBasedOnPhysics()
    {
        Entity::updateBasedOnPhysics();
        b2Vec2 position = b2Body_GetPosition(body);
        position.x = this->position.x; // Force the X position to remain constant (locked)
        b2Body_SetTransform(body, position, b2Body_GetRotation(body));
    }


    void Ship::startContact()
    {
        std::cout << "Ship start contact" << std::endl;
        m_jumping = false;
    }
}
