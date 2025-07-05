#pragma once
#include <string>
#include <glm/glm.hpp>
#include "Events.h"
#include "engine/Context.h"
#include "box2d/box2d.h"
#include <entt/entity/registry.hpp>

#include "VisualEffects.h"


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

    namespace audio
    {
        class AudioSystem;
    }

    namespace state
    {
        class StateManagementSystem;
    }

    enum class GameState
    {
        None,
        LevelSelect,
        EditMode,
        Menu,
        Level,
        PreviewWithScrolling,
        PreviewWithTesting
    };


    class Game
    {
    public:
        void run();
        [[nodiscard]] GLFWwindow* getWindow() const { return context_.getWindow(); }
        [[nodiscard]] b2WorldId getPhysicsWorld() const { return physics_world_; };
        [[nodiscard]] const context::Context& getContext() const { return context_; }
        context::Context& getContext() { return context_; }
        [[nodiscard]] float getDeltaTime() const { return delta_time_; };
        entt::registry& getRegistry() { return registry_; };
        [[nodiscard]] ui::UISystem* getUISystem() const { return ui_system_; };
        [[nodiscard]] physics::PhysicsSystem* getPhysicsSystem() const { return physics_system_; }
        [[nodiscard]] audio::AudioSystem* getAudioSystem() const { return audio_system_; };
        [[nodiscard]] state::StateManagementSystem* getStateManagement() const { return state_management_system_; };
        [[nodiscard]] entt::entity getPlayer() const { return player_; }
        void setPlayer(const entt::entity player) { player_ = player; }

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
        virtual ~Game();
        void updateDeltaTime();

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

        virtual void updateState();

        context::Context context_;

        float delta_time_ = 1.0f / 60;

        b2WorldId physics_world_;

        physics::PhysicsSystem* physics_system_;
        rendering::RenderingSystem* rendering_system_;
        ui::UISystem* ui_system_;
        audio::AudioSystem* audio_system_;
        state::StateManagementSystem* state_management_system_;


        entt::registry registry_;
        entt::entity player_;

    private:
        float lastFrameTime_ = 1.0f / 60;
    };
} // gl3
