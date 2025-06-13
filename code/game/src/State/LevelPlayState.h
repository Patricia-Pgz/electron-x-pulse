#pragma once
#include "engine/Game.h"
#include "InGameMenuSystem.h"
#include "InstructionUI.h"
#include "engine/levelloading/Objects.h"
#include "engine/stateManagement/GameState.h"
#include "engine/userInterface/UISystem.h"

namespace gl3::game::state
{
    class LevelPlayState final : public engine::state::GameState
    {
    public:
        explicit LevelPlayState(ui::InGameMenuSystem& menuUI, const int levelIndex, engine::Game& game)
            : menu_ui_(menuUI), level_index_(levelIndex), game_(game)
        {
        }

        void onEnter() override
        {
            loadLevel();
            menu_ui_.setActive(true);
            if (level_index_ == 0)
            {
                game_.getUISystem().getSubsystems(2).setActive(true); //deactivates itself after timer
            }
        }

        void onExit() override
        {
            menu_ui_.setActive(false);
        }

        void update(float dt) override
        {
        }

    private:
        void loadLevel();
        void startLevel(); //TODO alles nach links bewegen + Musik erst hier starten!!!
        void reloadLevel(); //TODO on Death
        ui::InGameMenuSystem& menu_ui_;
        std::vector<GameObject> initial_test_game_objects_;
        int level_index_ = -1;
        Level* current_level_ = nullptr;
        engine::Game& game_;
    };
}
