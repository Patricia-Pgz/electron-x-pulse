#include <stdexcept>
#include "engine/Game.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/rendering/RenderingSystem.h"
#include "engine/userInterface/UISystem.h"
#include "engine/audio/AudioSystem.h"
#include "engine/levelloading/LevelManager.h"
#include "engine/stateManagement/StateManagementSystem.h"


namespace gl3::engine
{
    using Context = context::Context;

    Game::Game(const int width, const int height, const std::string& title, const glm::vec3 camPos,
               const float camZoom): context(width, height, title, camPos, camZoom), physics_world(b2_nullWorldId),
                                     physics_system(new physics::PhysicsSystem(*this)),
                                     rendering_system((new rendering::RenderingSystem(*this))),
                                     ui_system(new ui::UISystem(*this)),
                                     audio_system(new audio::AudioSystem(*this)),
                                     state_management_system(new state::StateManagementSystem(*this)),
                                     player(entt::null)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }

        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        // We use worldDef to define our physics world
        worldDef.gravity = b2Vec2{0.f, -10.f};
        physics_world = b2CreateWorld(&worldDef);
        ui_system->initUI();
    }

    void Game::run()
    {
        onStartup.invoke(*this);
        start();
        //register ui subsystems in time to be able to update them
        registerUiSystems();
        //preload all textures
        rendering::TextureManager::loadTextures();
        //preload all level metadata files for preview
        levelLoading::LevelManager::loadAllMetaData();
        onAfterStartup.invoke(*this);
        context.run([&](Context& ctx)
        {
            onBeforeUpdate.invoke(*this);
            update(getWindow());
            updateState();
            updatePhysics();
            draw();
            updateUI();
            updateDeltaTime();
            //delete entities safely after updates
            ecs::EntityFactory::deleteMarkedEntities(registry);
            onAfterUpdate.invoke(*this);
        });
        onBeforeShutdown.invoke(*this);
        onShutdown.invoke(*this);
    }

    void Game::updateDeltaTime()
    {
        const auto frameTime = static_cast<float>(glfwGetTime());
        delta_time = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
    }

    Game::~Game()
    {
        glfwTerminate();
    }

    void Game::updatePhysics()
    {
        physics_system->runPhysicsStep();
    }

    void Game::draw()
    {
        rendering_system->draw();
    }

    void Game::updateUI()
    {
        ui_system->renderUI(delta_time);
    }

    void Game::updateState()
    {
        state_management_system->update(delta_time);
        audio_system->update();
    }
} // gl3
