#include "engine/Game.h"
#include <stdexcept>

namespace gl3::engine {
    using Context = context::Context;
    void Game::run() {
        onStartup.invoke(*this);
        start();
        onAfterStartup.invoke(*this);
        context.run([&](context::Context &ctx){
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

    void Game::calculateWindowBounds() //TODO in context
    {
        int width, height;
        glfwGetWindowSize(getWindow(), &width, &height);

        float halfWidth = (width / 2.0f) * zoom;
        float halfHeight = (height / 2.0f) * zoom;

        windowLeft = cameraPosition.x - halfWidth;
        windowRight = cameraPosition.x + halfWidth;
        windowBottom = cameraPosition.y - halfHeight;
        windowTop = cameraPosition.y + halfHeight;
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    //TODO implement scrolling with Camera (+ limit to 0-width/2 and song/levellength+width/2
    {
        auto gameInstance = static_cast<Game*>(glfwGetWindowUserPointer(window));
        //gameInstance->scroll_callback_fun(yoffset); TODO das und framebufersizecallback als events
    }

        Game::Game(int width, int height, const std::string& title, glm::vec3 camPos,
               float camZoom): context(width, height, title),cameraPosition(camPos), zoom(camZoom)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }
        glfwSetWindowUserPointer(getWindow(), this);
        glfwSetScrollCallback(getWindow(), scroll_callback);
        if (glGetError() != GL_NO_ERROR)
        {
            throw std::runtime_error("gl error");
        }
        calculateWindowBounds(); //TODO in context
        audio.init();
        audio.setGlobalVolume(0.1f);

        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        // We use worldDef to define our physics world
        worldDef.gravity = b2Vec2{0.f, -9.81f};
        physicsWorld = b2CreateWorld(&worldDef);
    }

    glm::mat4 Game::calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale)
    {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::lookAt(cameraPosition,
                                     cameraCenter,
                                     glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::ortho(windowLeft, windowRight, windowBottom, windowTop, 0.1f, 100.0f);

        return projection * view * model;
    }

    void Game::updateDeltaTime() {
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