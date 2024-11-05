#include "Ship.h"
#include "../Game.h"
#include "../Assets.h"


namespace gl3 {
    Ship::Ship(glm::vec3 position, float zRotation, glm::vec3 scale) : Entity(
            Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            Mesh({
                         0.5f, 0.025f, 0.0f,
                         0.0f, 0.3f, 0.0f,
                         -0.2f, 0.05f, 0.0f,

                         0.5f, -0.025f, 0.0f,
                         0.0f, -0.3f, 0.0f,
                         -0.2f, -0.05f, 0.0f
                 },
                 {0, 1, 2,
                  3, 4, 5}),
            position,
            zRotation,
            scale,
            {1, 1, 1, 1}) {

        audio.init();
        audio.setGlobalVolume(0.1f);
        firingSound.load(resolveAssetPath("audio/shot.mp3").string().c_str());
        firingSound.setSingleInstance(true);
    }

    void Ship::update(Game *game, float deltaTime) {
        auto window = game->getWindow();
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            zRotation += rotationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            zRotation -= rotationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position.y += sin(glm::radians(zRotation)) * translationSpeed * deltaTime;
            position.x += cos(glm::radians(zRotation)) * translationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position.y -= sin(glm::radians(zRotation)) * translationSpeed * deltaTime;
            position.x -= cos(glm::radians(zRotation)) * translationSpeed * deltaTime;
        }

        countdownUntilNextShot -= deltaTime;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && countdownUntilNextShot <= 0)  {
            audio.play(firingSound);
            auto missile = std::make_unique<Missile>(game);
            missile->setPosition(this->getPosition());
            missile->setZRotation(this->getZRotation() - 90.0f);
            missile->setScale(glm::vec3(0.05f, 0.05f, 0.05f));
            missiles.push_back(std::move(missile));
            countdownUntilNextShot = timeBetweenShots;
        }
        for (auto &m: missiles) {
            m->update(game, deltaTime);
        }
        if (missiles.size() >= 100) {
            missiles.erase(missiles.begin());
        }
    }

    void Ship::draw(Game *game) {
        Entity::draw(game);
        for (auto &m: missiles) {
            m->draw(game);
        }
    }
}
