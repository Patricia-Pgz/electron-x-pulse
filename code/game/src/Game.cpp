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
#include "engine/audio/AudioSystem.h"
#include "State/LevelPlayState.h"

namespace gl3::game
{
    Game::Game(const int width, const int height, const std::string& title, const glm::vec3& camPos,
               const float camZoom)
        : engine::Game(width, height, title, camPos, camZoom), game_state_manager_(new GameStateManager(*this)),
          player_input_system_(*this)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::context::onMouseScrollEvent>().connect<&
            Game::on_mouse_scroll>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::LevelStartEvent>().connect<&
            Game::on_lvl_start>(this);
    }

    void Game::on_mouse_scroll(engine::context::onMouseScrollEvent& event)
    {
        /*if (currentGameState != engine::GameState::PreviewWithScrolling) return;
        float cameraX = 0.0f; /*cameraPosition.x;#1#
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
        context_.setCameraCenter(newCameraCenter);*/
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

    void Game::on_lvl_start(const engine::ecs::LevelStartEvent& event)
    {
        player_ = event.player; //TODO brauch ich den hier überhaupt?
    }

    void Game::start()
    {
        //TODO gameconfig laden?
    }

    void Game::update(GLFWwindow* window)
    //TODO deactivate components, that are out of left screen boundary (RenderComp, Physics)
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
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::LevelStartEvent>().disconnect<&
            Game::on_lvl_start>(this);
    }
}
