#pragma once
#include "engine/Game.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/levelloading/Objects.h"
#include "engine/stateManagement/GameState.h"
#include "engine/userInterface/UISystem.h"
#include "ui/FinishUI.h"
#include "ui/InGameMenuUI.h"
#include "ui/InstructionUI.h"

namespace gl3::game::state
{
    struct LevelBackgroundConfig
    {
        float centerX;
        float windowWidth;

        float groundCenterY;
        float groundHeight;

        float skyCenterY;
        float skyHeight;
    };

    class LevelPlayState final : public engine::state::GameState
    {
    public:
        explicit LevelPlayState(engine::Game& game, const int levelIndex, const bool editMode)
            : GameState(game), edit_mode(editMode), level_index(levelIndex)
        {
            afterPhysicsStepHandle = game.onBeforeUpdate.addListener([&](engine::Game& game_)
            {
                onPhysicsStepDone();
            });
            const auto& topLvlUI = game.getUISystem();
            menu_ui = topLvlUI->getSubsystem<ui::InGameMenuUI>();
            instruction_ui = topLvlUI->getSubsystem<ui::InstructionUI>();
            finish_ui = topLvlUI->getSubsystem<ui::FinishUI>();
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().connect<&
                LevelPlayState::onPlayerDeath>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::RestartLevelEvent>().connect<&
                LevelPlayState::onRestartLevel>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().connect<&
                LevelPlayState::onPauseEvent>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::context::WindowBoundsRecomputeEvent>().connect<&
                LevelPlayState::onWindowSizeChange>(this);
        }

        ~LevelPlayState() override
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().disconnect<&
                LevelPlayState::onPlayerDeath>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::RestartLevelEvent>().disconnect<&
                LevelPlayState::onRestartLevel>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().disconnect<&
                LevelPlayState::onPauseEvent>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::context::WindowBoundsRecomputeEvent>().disconnect<&
                LevelPlayState::onWindowSizeChange>(this);
            game.onBeforeUpdate.removeListener(afterPhysicsStepHandle);
        }

        void onEnter() override
        {
            menu_ui->setActive(true);
            if (level_index == 0) //Tutorial
            {
                instruction_ui->setActive(true); //deactivates itself after timer
            }
            loadLevel();
        }

        void onExit() override
        {
            unloadLevel();
        }

        void update(float deltaTime) override;

    private:
        void loadLevel();
        void moveObjects(bool move) const;
        void pauseOrStartLevel(bool pause);
        void setSystemsActive(bool setActive) const;
        void reloadLevel();
        void unloadLevel();
        void delayLevelEnd(float deltaTime);

        void onPlayerDeath(const engine::ecs::PlayerDeath& event);
        void onWindowSizeChange(const engine::context::WindowBoundsRecomputeEvent& event) const;
        [[nodiscard]] LevelBackgroundConfig updateBackgrounds(const std::vector<float>& windowBounds) const;
        void onRestartLevel(const engine::ui::RestartLevelEvent& event);
        void onPauseEvent(const engine::ui::PauseLevelEvent& event);
        void onPhysicsStepDone();
        void startLevel();

        float levelTime = 0.f;
        std::list<std::function<void(engine::Game&)>>::iterator afterPhysicsStepHandle;
        ui::InGameMenuUI* menu_ui = nullptr;
        ui::FinishUI* finish_ui = nullptr;
        ui::InstructionUI* instruction_ui = nullptr;
        engine::audio::AudioConfig* audio_config = nullptr;
        bool reset_level = false;
        bool edit_mode = false;
        bool paused = true;
        bool level_instantiated = false;
        bool timer_active = false;
        bool transition_triggered = false;
        float timer = 2.f;
        int level_index = -1;
        Level* current_level = nullptr;
        entt::entity current_player = entt::null;
    };
}
