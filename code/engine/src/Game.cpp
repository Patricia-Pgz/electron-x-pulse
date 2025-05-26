#include <stdexcept>
#include "engine/Game.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/rendering/RenderingSystem.h"
#include "engine/userInterface/TopLvlUISystem.h"


namespace gl3::engine
{
    using Context = context::Context;

    void Game::run()
    {
        onStartup.invoke(*this);
        start();
        onAfterStartup.invoke(*this);
        context_.run([&](Context& ctx)
        {
            onBeforeUpdate.invoke(*this);
            update(getWindow());
            updatePhysics();
            draw();
            updateUI();
            updateDeltaTime();
            onAfterUpdate.invoke(*this);
        });
        onBeforeShutdown.invoke(*this);
        onShutdown.invoke(*this);
    }

    Game::Game(const int width, const int height, const std::string& title, const glm::vec3 camPos,
               const float camZoom): context_(width, height, title, camPos, camZoom), physics_world_(b2_nullWorldId),
                                     physics_system_(new physics::PhysicsSystem(*this)),
                                     rendering_system_((new rendering::RenderingSystem(*this))),
                                     top_lvl_ui_system_(new ui::TopLvlUISystem(*this)), //TODO Lieber in game?
                                     player_(entt::null)
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
        physics_world_ = b2CreateWorld(&worldDef);
        top_lvl_ui_system_->initUI();
    }

    void Game::updateDeltaTime()
    {
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime_;
        lastFrameTime_ = frameTime;
    }

    Game::~Game()
    {
        glfwTerminate();
    }

    void Game::updatePhysics()
    {
        physics_system_->runPhysicsStep();
        //TODO implement way to deactivate physics / stop game / only update active physics comps
    }

    void Game::draw()
    {
        rendering_system_->draw();
    }

    void Game::updateUI()
    {
        top_lvl_ui_system_->renderUI();
    }
} // gl3
