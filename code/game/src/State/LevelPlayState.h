#pragma once
#include "./InGameMenuSystem.h"
#include "engine/Game.h"
#include "InstructionUI.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/levelloading/Objects.h"
#include "engine/stateManagement/GameState.h"
#include "engine/userInterface/UISystem.h"

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
        explicit LevelPlayState(ui::InGameMenuSystem& menuUI, const int levelIndex, engine::Game& game)
            : game_(game), menu_ui_(menuUI), level_index_(levelIndex)
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::context::WindowResizeEvent>().connect<&
                LevelPlayState::onWindowResize>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().connect<&
                LevelPlayState::reloadLevel>(this);
        }

        ~LevelPlayState() override
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::context::WindowResizeEvent>().disconnect<&
                LevelPlayState::onWindowResize>(this);
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().disconnect<&
                LevelPlayState::reloadLevel>(this);
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
        void moveObjects() const;
        void startLevel() const;
        [[nodiscard]] LevelBackgroundConfig calculateBackgrounds() const;
        void applyBackgroundEntityTransform(LevelBackgroundConfig& bgConfig) const;
        void updateBackgroundEntities() const;
        void onWindowResize(const engine::context::WindowResizeEvent& evt) const;
        void reloadLevel();
        void unloadLevel();
        engine::Game& game_;
        ui::InGameMenuSystem& menu_ui_;
        bool isLevelInstantiated = false;
        int level_index_ = -1;
        Level* current_level_ = nullptr;
        entt::entity current_player_ = entt::null;
        std::vector<entt::entity> backgroundEntities;
    };
}
