#include "Game.h"
#include <random>

#include "InGameMenuSystem.h"
#include "PlayerInputSystem.h"
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
        engine::rendering::TextureManager::loadTextures(); //TODO in engine?
        game_config_.ground_level = -1;
        GameObject groundObj = {
            glm::vec3(0, game_config_.ground_level - game_config_.ground_height / 2, 0.0f), glm::vec4(0.25, 0.27, 1, 1),
            "ground",
            false
        };
        const auto& ground = engine::ecs::EntityFactory::createDefaultEntity(groundObj,
                                                                             registry_, physics_world_);
        engine::ecs::EntityFactory::setScale(registry_, ground, glm::vec3(40.f, game_config_.ground_height, 0.f));

        GameObject playerObj = {
            glm::vec3(game_config_.initial_player_position_x, 0.f, 0), glm::vec4(0.25f, 0.25f, 0.25f, 1.0f), "player",
            false, "geometry-dash"
        };
        player_ = engine::ecs::EntityFactory::createDefaultEntity(playerObj,
                                                                  registry_, physics_world_);
        engine::ecs::EntityFactory::setScale(registry_, player_, glm::vec3(1.f, 1.f, 1.f));
    }

    void Game::update(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::GameStateChange(engine::GameState::Level));
        }
        player_input_system_.update(player_);
    }

    //TODO delete entities when levelReload/Load/wechsel

    void Game::registerUiSystems()
    {
        ui_system_->registerSubsystem<engine::levelLoading::LevelSelectUISystem>();
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
