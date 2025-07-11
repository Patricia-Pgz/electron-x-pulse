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
    GameStateManager::GameStateManager(Game& game) : game(game)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().connect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::EditeModeButtonPress>().connect<&
            GameStateManager::onEditModeChange>(this);
        onUIInitHandle = game.getUISystem()->onInitialized.addListener([this]()
        {
            onUiInitialized();
        });
    }

    GameStateManager::~GameStateManager()
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().disconnect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::EditeModeButtonPress>().disconnect<&
            GameStateManager::onEditModeChange>(this);
        game.getUISystem()->onInitialized.removeListener(onUIInitHandle);
    }

    void GameStateManager::onUiInitialized() const
    {
        game.getStateManagement()->pushState<state::LevelSelectState>(true,
                                                                      game);
        game.getAudioSystem()->loadOneShot("win", "win.wav");
        game.getAudioSystem()->loadOneShot("crash", "crash.wav");
    }

    void GameStateManager::onEditModeChange(const engine::ui::EditeModeButtonPress& event)
    {
        is_edit_mode = event.edit;
    }

    void GameStateManager::onGameStateChange(const engine::ecs::GameStateChange& newState)
    {
        auto* stateSystem = game.getStateManagement();
        previous_game_state = current_game_state;
        current_game_state = newState.newGameState;

        if (current_game_state == engine::GameState::LevelSelect)
        {
            if (dynamic_cast<state::EditorState*>(stateSystem->getCurrentState()) != nullptr)
            {
                //was in edit mode until just now
                stateSystem->popState(false);
                //pop EditorState without starting LevelPlayState, because it is already running as well -> enterNext = false
                //now there is only the LevelPlayState in the state stack -> safe to change to LevelSelect now
            }
            stateSystem->changeState<state::LevelSelectState>(game);
            return;
        }

        if (current_game_state == engine::GameState::Level && previous_game_state != engine::GameState::Level)
        {
            stateSystem->changeState<state::LevelPlayState>(game, newState.newLevelIndex, is_edit_mode);
            if (!is_edit_mode) return;

            stateSystem->pushState<state::EditorState>(false, game);
        }
    }
} // gl3
