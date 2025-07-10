#include "Game.h"
#include <random>

#include "PlayerInputSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/userInterface/UISystem.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "GameStateManager.h"
#include "engine/audio/AudioSystem.h"
#include "engine/physics/PhysicsSystem.h"
#include "states/LevelPlayState.h"
#include "ui/FinishUI.h"
#include "ui/InGameMenuUI.h"
#include "ui/InstructionUI.h"

namespace gl3::game
{
    Game::Game(const int width, const int height, const std::string& title, const glm::vec3& camPos,
               const float camZoom)
        : engine::Game(width, height, title, camPos, camZoom), game_state_manager_(new GameStateManager(*this)),
          player_input_system_(new input::PlayerInputSystem(*this))
    {
    }

    void Game::start()
    {
    }

    void Game::update(GLFWwindow* window)
    {
        player_input_system_->update();
    }

    void Game::registerUiSystems()
    {
        ui_system_->registerSubsystem<engine::levelLoading::LevelSelectUISystem>();
        ui_system_->registerSubsystem<ui::InGameMenuUI>();
        ui_system_->registerSubsystem<ui::InstructionUI>();
        ui_system_->registerSubsystem<ui::FinishUI>();
        ui_system_->registerSubsystem<engine::editor::EditorUISystem>();
    }
}
