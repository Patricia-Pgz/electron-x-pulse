#include "GameStateManager.h"

#include "engine/ecs/EntityFactory.h"
#include "engine/physics/PhysicsSystem.h"
#include "states/LevelSelectState.h"
#include "engine/stateManagement/StateManagerSystem.h"
#include "engine/userInterface/UISystem.h"
#include "states/LevelPlayState.h"

namespace gl3::game
{
    GameStateManager::GameStateManager(Game& game) : game_(game)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().connect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().connect<&
            GameStateManager::onPauseLevel>(this);
        onUIInitHandle = game_.getUISystem()->onInitialized.addListener([this]()
        {
            onUiInitialized();
        });
    }

    GameStateManager::~GameStateManager()
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().disconnect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().disconnect<&
            GameStateManager::onPauseLevel>(this);
        game_.getUISystem()->onInitialized.removeListener(onUIInitHandle);
    }

    void GameStateManager::onUiInitialized() const
    {
        game_.getStateManagement()->pushState<state::LevelSelectState>(
            game_);
    }

    void GameStateManager::onGameStateChange(const engine::ecs::GameStateChange& newState) const
    {
        auto* stateSystem = game_.getStateManagement();

        if (newState.newGameState == engine::GameState::LevelSelect)
        {
            game_.getPhysicsSystem()->setActive(false);
            stateSystem->changeState<state::LevelSelectState>(game_);
            return;
        }

        if (newState.newGameState == engine::GameState::Level && previous_game_state_ != engine::GameState::Level)
        {
            stateSystem->changeState<state::LevelPlayState>(game_, newState.newLevelIndex);
            return;
        }
    }

    void GameStateManager::onPauseLevel(const engine::ui::PauseLevelEvent& event) const
    {
        if (event.pauseLevel)
        {
            game_.getPhysicsSystem()->setActive(false);
            game_.getPlayerInputSystem()->setActive(false);
        }
        else
        {
            game_.getPhysicsSystem()->setActive(true);
            game_.getPlayerInputSystem()->setActive(true);
        }
    }
} // gl3
