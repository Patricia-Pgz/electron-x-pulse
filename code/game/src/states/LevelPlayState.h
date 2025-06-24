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
        float center_x;
        float windowWidth;

        float ground_center_y;
        float ground_height;

        float sky_center_y;
        float sky_height;
    };

    class LevelPlayState final : public engine::state::GameState
    {
    public:
        explicit LevelPlayState(engine::Game& game, const int levelIndex, const bool editMode)
            : GameState(game), edit_mode_(editMode), level_index_(levelIndex)
        {
            const auto& topLvlUI = game_.getUISystem();
            menu_ui_ = topLvlUI->getSubsystem<ui::InGameMenuUI>();
            instruction_ui_ = topLvlUI->getSubsystem<ui::InstructionUI>();
            finish_ui_ = topLvlUI->getSubsystem<ui::FinishUI>();

            engine::ecs::EventDispatcher::dispatcher.sink<engine::context::WindowResizeEvent>().connect<&
                LevelPlayState::onWindowResize>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().connect<&
                LevelPlayState::onPlayerDeath>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::RestartLevelEvent>().connect<&
                LevelPlayState::reloadLevel>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().connect<&
                LevelPlayState::onPauseEvent>(this);
        }

        ~LevelPlayState() override
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::context::WindowResizeEvent>().disconnect<&
                LevelPlayState::onWindowResize>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().disconnect<&
                LevelPlayState::onPlayerDeath>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::RestartLevelEvent>().disconnect<&
                LevelPlayState::reloadLevel>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().disconnect<&
                LevelPlayState::onPauseEvent>(this);
        }

        void onEnter() override
        {
            loadLevel();
            menu_ui_->setActive(true);
            if (level_index_ == 0) //Tutorial
            {
                instruction_ui_->setActive(true); //deactivates itself after timer
            }
        }

        void onExit() override
        {
            unloadLevel();
        }

        void update(const float deltaTime) override
        {
            if (!level_instantiated_)
            {
                return;
            }
            if (edit_mode_ && glfwGetKey(game_.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                game_.getAudioSystem()->playCurrentAudio();
                pauseOrStartLevel(false);
            }
            delayLevelEnd(deltaTime);
        }

    private:
        void loadLevel();
        void moveObjects() const;
        void stopMovingObjects() const;
        void pauseOrStartLevel(bool stop) const;
        void reloadLevel();
        void unloadLevel();
        void delayLevelEnd(float deltaTime);

        void onWindowResize(const engine::context::WindowResizeEvent& evt) const;
        void onPlayerDeath(const engine::ecs::PlayerDeath& event);
        void onPauseEvent(const engine::ui::PauseLevelEvent& event) const;

        [[nodiscard]] LevelBackgroundConfig calculateBackgrounds() const;
        void applyBackgroundEntityTransform(LevelBackgroundConfig& bgConfig, entt::entity entity) const;
        void updateBackgroundEntity(LevelBackgroundConfig& bgConfig, entt::entity entity) const;

        ui::InGameMenuUI* menu_ui_;
        ui::FinishUI* finish_ui_;
        ui::InstructionUI* instruction_ui_;
        engine::audio::AudioConfig* audio_config_ = nullptr;
        bool edit_mode_ = false;
        bool level_instantiated_ = false;
        bool timer_active_ = false;
        bool transition_triggered_ = false;
        float timer_ = 1.f;
        int level_index_ = -1;
        Level* current_level_ = nullptr;
        entt::entity current_player_ = entt::null;
    };
}
