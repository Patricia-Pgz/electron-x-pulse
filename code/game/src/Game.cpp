#include "Game.h"
#include <random>
#include <iostream>

#include "InGameMenuSystem.h"
#include "engine/Assets.h"
#include "PlayerInputSystem.h"
#include "engine/AudioAnalysis.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/userInterface/UISystem.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "InstructionUI.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "GameStateManager.h"

namespace gl3::game
{
    Game::Game(const int width, const int height, const std::string& title, const glm::vec3& camPos,
               const float camZoom)
        : engine::Game(width, height, title, camPos, camZoom), game_state_manager_(new GameStateManager(*this)),
          player_input_system_(*this)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::context::onMouseScrollEvent>().connect<&
            Game::on_mouse_scroll>(this);
    }

    void Game::on_mouse_scroll(engine::context::onMouseScrollEvent& event)
    {
        if (currentGameState != engine::GameState::PreviewWithScrolling) return;
        float cameraX = 0.0f; /*cameraPosition.x;*/
        float scrollSpeed = 0.5f;
        float minScrollX = 0.0f; // Minimum scroll limit
        int width, height;
        glfwGetWindowSize(getWindow(), &width, &height);
        float maxScrollX = (game_config_.level_length + static_cast<float>(width) / 2 * context_.getCurrentZoom() -
            game_config_.initial_player_position_x);
        // Maximum scroll limit

        cameraX = static_cast<float>(event.yOffset) * scrollSpeed;
        if (cameraX < minScrollX) cameraX = minScrollX;
        if (cameraX > maxScrollX) cameraX = maxScrollX;

        moveEntitiesScrolling();

        auto newCameraPosition = glm::vec3(cameraX, context_.getCameraPos().y, context_.getCameraPos().z);
        auto newCameraCenter = glm::vec3(cameraX, context_.getCameraCenter().y, context_.getCameraCenter().z);
        context_.setCameraPos(newCameraPosition);
        context_.setCameraCenter(newCameraCenter);
    }


    void Game::moveEntitiesScrolling()
    {
        auto entities = registry_.view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();

        for (auto entity : entities)
        {
            auto& tag_component = entities.get<engine::ecs::TagComponent>(entity);
            auto& physics_component = entities.get<engine::ecs::PhysicsComponent>(entity);
            if (tag_component.tag != "beat" && tag_component.tag != "timeline")
            {
                b2Body_SetTransform(physics_component.body,
                                    b2Vec2(b2Body_GetPosition(physics_component.body).x + context_.getCameraPos().x,
                                           b2Body_GetPosition(physics_component.body).y),
                                    b2Body_GetRotation(physics_component.body));
            }
        }
    }

    void Game::start()
    {
        engine::rendering::TextureManager::loadTextures();
        game_config_.ground_level = -1;
        const auto& ground = engine::ecs::EntityFactory::createDefaultEntity(
            registry_, glm::vec3(0, game_config_.ground_level - game_config_.ground_height / 2, 0.0f),
            glm::vec4(0.25, 0.27, 1, 1),
            "ground",
            physics_world_, false);
        engine::ecs::EntityFactory::setScale(registry_, ground, glm::vec3(40.f, game_config_.ground_height, 0.f));

        player_ = engine::ecs::EntityFactory::createDefaultEntity(
            registry_, glm::vec3(game_config_.initial_player_position_x, 0.f, 0),
            glm::vec4(0.25f, 0.25f, 0.25f, 1.0f), "player", physics_world_, false,
            &engine::rendering::TextureManager::get("geometry-dash"));
        engine::ecs::EntityFactory::setScale(registry_, player_, glm::vec3(1.f, 1.f, 1.f));

        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(engine::resolveAssetPath("audio/SensesShort.wav").c_str());
        backgroundMusic->setLooping(false);
        game_config_.current_audio_length = static_cast<float>(backgroundMusic->getLength());
    }

    void Game::update(GLFWwindow* window)
    {
        /*if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            currentGameState = engine::GameState::Level;
            //TODO Preview funktioniert nicht mehr, prüfen wie ich entities generiere!!
        }

        if (currentGameState != previousGameState)
        {
            previousGameState = currentGameState;
            onGameStateChange();
        }*/
        player_input_system_.update(player_);
    }

    //TODO delete entities when levelReload/Load/wechsel

    void Game::registerUiSystems()
    {
        //ui_system_->registerSubsystem<engine::levelLoading::LevelSelectUISystem>();
        ui_system_->registerSubsystem<ui::InGameMenuSystem>();
        ui_system_->registerSubsystem<ui::InstructionUI>();
        //ui_system_->registerSubsystem<engine::editor::EditorUISystem>();
    }

    Game::~Game()
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::context::onMouseScrollEvent>().disconnect<&
            Game::on_mouse_scroll>(this);
    }
}
