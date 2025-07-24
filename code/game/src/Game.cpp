#include "Game.h"

#include "engine/userInterface/UISystem.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "GameStateManager.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EntityFactory.h"
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

    ///Update needed custom systems each frame (here: PlayerInputSystem) and check if player is still in view
    void Game::update(GLFWwindow* window)
    {
        //player death if they move out of visible view
        if (registry.valid(player) && !is_paused)
        {
            const auto& playerTransform = registry.get<engine::ecs::TransformComponent>(player);
            const auto& windowBounds = context.getWorldWindowBounds();
            //Trigger death event if player leaves window
            if (!context.isInVisibleWindow(playerTransform.position,
                                           playerTransform.scale, 1.5f) || playerTransform.position.y > windowBounds[2] + 1.f || playerTransform.position.y < windowBounds[3] - 1.f)
            {
                engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayerDeath{});
            }
        }
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
