#include "Game.h"

#include "engine/userInterface/UISystem.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "GameStateManager.h"
#include "engine/audio/AudioSystem.h"
#include "ui/FinishUI.h"
#include "ui/InGameMenuUI.h"
#include "ui/InstructionUI.h"

namespace gl3::game
{
    Game::Game(const int width, const int height, const std::string& title, const glm::vec3& camPos,
               const float camZoom)
        : engine::Game(width, height, title, camPos, camZoom), game_state_manager(new GameStateManager(*this)),
          player_input_system(new input::PlayerInputSystem(*this))
    {
    }

    ///Update needed custom systems each frame (here: PlayerInputSystem)
    void Game::update(GLFWwindow* window)
    {
        player_input_system->update();
    }

    ///Preregister all UI systems used in the game.
    void Game::registerUiSystems()
    {
        ui_system->registerSubsystem<engine::levelLoading::LevelSelectUISystem>();
        ui_system->registerSubsystem<ui::InGameMenuUI>();
        ui_system->registerSubsystem<ui::InstructionUI>();
        ui_system->registerSubsystem<ui::FinishUI>();
        ui_system->registerSubsystem<engine::editor::EditorUISystem>();
    }
}
