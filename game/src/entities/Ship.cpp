#include "Ship.h"

#include <iostream>

#include "../Game.h"
#include "../Assets.h"
#include <cmath>


namespace gl3 {
    Ship::Ship(glm::vec3 position, float zRotation, glm::vec3 scale, b2WorldId physicsWorld) : Entity(
            Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            Mesh({
                -0.5f, 0.5f, 0.0f,
                -0.5f, -0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
                0.5f, -0.5f, 0.0f
                 },
                 {0, 1, 2,
                     1, 2, 3}),
            position,
            zRotation,
            scale,
            {1, 1, 1, 1},
            physicsWorld)
    {
        audio.init();
        audio.setGlobalVolume(0.1f);
        firingSound.load(resolveAssetPath("audio/shot.mp3").string().c_str());
        firingSound.setSingleInstance(true);
        Ship::createPhysicsBody();
    }

    void Ship::update(Game *game, float deltaTime) {
        auto window = game->getWindow();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        auto body = getBody();

        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && static_cast<int>(std::round(position.x)) % 2 == 0) { //Jump
            b2Body_ApplyLinearImpulseToCenter(body, b2Vec2{0.0f, m_jumpImpulse}, true);
        }

    }

    void Ship::draw(Game *game) {
        Entity::draw(game);
    }

    void Ship::createPhysicsBody()
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        //bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
        bodyDef.fixedRotation = true;
        bodyDef.linearDamping = 0.5f;

        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.friction = 0.1f;
        shapeDef.restitution = 0.1f;
        shapeDef.enableContactEvents = true;

        b2Polygon box = b2MakeBox(scale.x * 0.5f, scale.y * 0.5f);
        shape = b2CreatePolygonShape(body, &shapeDef, &box);
    }

    void Ship::updateBasedOnPhysics()
    {
        Entity::updateBasedOnPhysics();
    }


    void Ship::startContact() {
        std::cout << "Ship start contact" << std::endl;
    }
}
