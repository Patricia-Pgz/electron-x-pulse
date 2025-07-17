#include <stdexcept>
#include "engine/Game.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/rendering/RenderingSystem.h"
#include "engine/userInterface/UISystem.h"
#include "engine/audio/AudioSystem.h"
#include "engine/levelloading/LevelManager.h"
#include "engine/stateManagement/StateManagerSystem.h"


namespace gl3::engine
{
    using Context = context::Context;

    Game::Game(const int width, const int height, const std::string& title, const glm::vec3 camPos,
               const float camZoom): context_(width, height, title, camPos, camZoom), physics_world_(b2_nullWorldId),
                                     physics_system_(new physics::PhysicsSystem(*this)),
                                     rendering_system_((new rendering::RenderingSystem(*this))),
                                     ui_system_(new ui::UISystem(*this)),
                                     audio_system_(new audio::AudioSystem(*this)),
                                     state_management_system_(new state::StateManagementSystem(*this)),
                                     player_(entt::null)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }

        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        // We use worldDef to define our physics world
        worldDef.gravity = b2Vec2{0.f, -10.f};
        physics_world_ = b2CreateWorld(&worldDef);
        ui_system_->initUI();
    }

    void Game::run()
    {
        onStartup.invoke(*this);
        start();
        registerUiSystems();
        rendering::TextureManager::loadTextures();
        levelLoading::LevelManager::loadAllMetaData();
        onAfterStartup.invoke(*this);
        context_.run([&](Context& ctx)
        {
            onBeforeUpdate.invoke(*this);
            update(getWindow());
            updateState();
            updatePhysics();
            draw();
            updateUI();
            updateDeltaTime();
            ecs::EntityFactory::deleteMarkedEntities(registry_);
            onAfterUpdate.invoke(*this);
        });
        onBeforeShutdown.invoke(*this);
        onShutdown.invoke(*this);
    }

    void Game::updateDeltaTime()
    {
        const auto frameTime = static_cast<float>(glfwGetTime());
        delta_time_ = frameTime - lastFrameTime_;
        lastFrameTime_ = frameTime;
    }

    Game::~Game()
    {
        glfwTerminate();
    }

    void Game::updatePhysics()
    {
        physics_system_->runPhysicsStep();
    }

    void Game::draw()
    {
        rendering_system_->draw();
    }

    void Game::updateUI()
    {
        ui_system_->renderUI();
    }

    void Game::updateState()
    {
        state_management_system_->update();
        audio_system_->update();
    }
} // gl3
