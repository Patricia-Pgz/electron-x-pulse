#include "engine/Game.h"
#include <stdexcept>

namespace gl3::engine
{
    using Context = context::Context;

    void Game::run()
    {
        onStartup.invoke(*this);
        start();
        onAfterStartup.invoke(*this);
        context.run([&](Context& ctx)
        {
            onBeforeUpdate.invoke(*this);
            update(getWindow());
            updatePhysics();
            draw();
            updateDeltaTime();
            onAfterUpdate.invoke(*this);
        });
        onBeforeShutdown.invoke(*this);
        onShutdown.invoke(*this);
    }

    Game::Game(const int width, const int height, const std::string& title, const glm::vec3 camPos,
               const float camZoom): context(width, height, title, camPos, camZoom)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }


        audio.init();
        audio.setGlobalVolume(0.1f);

        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        // We use worldDef to define our physics world
        worldDef.gravity = b2Vec2{0.f, -9.81f};
        physicsWorld = b2CreateWorld(&worldDef);
    }

    glm::mat4 Game::calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale) const
    {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::lookAt(context.getCameraPos(),
                                     context.getCameraCenter(),
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        auto windowBounds = context.getWindowBounds();
        glm::mat4 projection = glm::ortho(windowBounds[0], windowBounds[1], windowBounds[2], windowBounds[3], 0.1f,
                                          100.0f);

        return projection * view * model;
    }

    void Game::updateDeltaTime()
    {
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
    }

    b2WorldId Game::getPhysicsWorld() const
    {
        return physicsWorld;
    }

    Game::~Game()
    {
        glfwTerminate();
    }
} // gl3
