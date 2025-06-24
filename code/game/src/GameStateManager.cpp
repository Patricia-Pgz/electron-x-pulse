#include "GameStateManager.h"

#include "engine/ecs/EntityFactory.h"
#include "engine/physics/PhysicsSystem.h"
#include "states/LevelSelectState.h"
#include "engine/stateManagement/StateManagerSystem.h"
#include "engine/userInterface/UISystem.h"
#include "states/EditorState.h"
#include "states/LevelPlayState.h"

namespace gl3::game
{
    GameStateManager::GameStateManager(Game& game) : game_(game)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().connect<&
            GameStateManager::onGameStateChange>(this);
        onUIInitHandle = game_.getUISystem()->onInitialized.addListener([this]()
        {
            onUiInitialized();
        });
    }

    GameStateManager::~GameStateManager()
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().disconnect<&
            GameStateManager::onGameStateChange>(this);
        game_.getUISystem()->onInitialized.removeListener(onUIInitHandle);
    }

    void GameStateManager::onUiInitialized() const
    {
        game_.getStateManagement()->pushState<state::LevelSelectState>(true,
                                                                       game_);
        game_.getAudioSystem()->loadOneShot("win", "win.wav");
        game_.getAudioSystem()->loadOneShot("crash", "crash.wav");
    }

    void GameStateManager::onGameStateChange(const engine::ecs::GameStateChange& newState)
    {
        auto* stateSystem = game_.getStateManagement();
        previous_game_state_ = current_game_state_;
        current_game_state_ = newState.newGameState;

        if (current_game_state_ == engine::GameState::LevelSelect)
        {
            if (dynamic_cast<state::EditorState*>(stateSystem->getCurrentState()) != nullptr)
            {
                //was in edit mode until just now
                stateSystem->popState(false);
                //pop EditorState without starting LevelPlayState, because it is already running as well -> enterNext = false
                //now there is only the LevelPlayState in the state stack -> safe to change to LevelSelect now
            }
            stateSystem->changeState<state::LevelSelectState>(game_);
            return;
        }

        if (current_game_state_ == engine::GameState::Level && previous_game_state_ != engine::GameState::Level)
        {
            stateSystem->changeState<state::LevelPlayState>(game_, newState.newLevelIndex,
                                                            previous_game_state_ == engine::GameState::EditMode);
            if (previous_game_state_ != engine::GameState::EditMode) return;

            stateSystem->pushState<state::EditorState>(false, game_);
            return;
        }
    }
} // gl3
