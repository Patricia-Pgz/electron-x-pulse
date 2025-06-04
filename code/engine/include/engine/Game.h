#pragma once
#include <string>
#include <glm/glm.hpp>
#include <soloud.h>
#include "Events.h"
#include "engine/Context.h"
#include "box2d/box2d.h"
#include <entt/entity/registry.hpp>

#include "userInterface/UIEvents.h"


namespace gl3::engine
{
    namespace physics
    {
        class PhysicsSystem;
    }

    namespace rendering
    {
        class RenderingSystem;
    }

    namespace ui
    {
        class UISystem;
    }

    enum class GameState
    {
        None,
        LevelSelect,
        Menu,
        Level,
        PreviewWithScrolling,
        PreviewWithTesting
    };

    struct GameConfig
    {
        float initial_player_position_x = -2.0f;

        float velocity_multiplier = -2.f;
        float bpm = 0.0f;
        float seconds_per_beat = 1.f;

        float level_length = 0.0f;
        float current_audio_length = 0.f; //TODO wird neu berechnet, wenn anderes lvl = song ausgewählt

        float ground_level = 0.f;
        float ground_height = 4.0f;

        [[nodiscard]] float level_speed() const
        {
            return velocity_multiplier / seconds_per_beat;
        }
    };


    class Game
    {
    public:
        void run();
        [[nodiscard]] GLFWwindow* getWindow() const { return context_.getWindow(); }
        [[nodiscard]] b2WorldId getPhysicsWorld() const { return physics_world_; };
        [[nodiscard]] const context::Context& getContext() const { return context_; }
        [[nodiscard]] float getDeltaTime() const { return delta_time_; };
        entt::registry& getRegistry() { return registry_; };
        [[nodiscard]] entt::entity getPlayer() const { return player_; }
        [[nodiscard]] GameConfig& getCurrentConfig() { return game_config_; }


        using event_t = events::Event<Game, Game&>;

        event_t onStartup;
        event_t onAfterStartup;
        event_t onBeforeUpdate;
        event_t onAfterUpdate;
        event_t onBeforeShutdown;
        event_t onShutdown;

        GameState currentGameState = GameState::Menu;

    protected:
        Game(int width, int height, const std::string& title, glm::vec3 camPos, float camZoom);
        void onGlobalVolumeChanged(const ui::VolumeChange& event);
        void updateDeltaTime();
        virtual ~Game();

        virtual void start()
        {
        };

        virtual void update(GLFWwindow* window)
        {
        };

        virtual void updatePhysics();

        virtual void draw();


        virtual void registerUiSystems()
        {
        };
        virtual void updateUI();

        context::Context context_;

        SoLoud::Soloud audio_;
        float global_volume_ = 1.0f;

        float delta_time_ = 1.0f / 60;

        b2WorldId physics_world_;

        physics::PhysicsSystem* physics_system_;
        rendering::RenderingSystem* rendering_system_;
        ui::UISystem* ui_system_;

        entt::registry registry_;
        entt::entity player_;

        GameConfig game_config_;

    private:
        float lastFrameTime_ = 1.0f / 60;
    };
} // gl3

