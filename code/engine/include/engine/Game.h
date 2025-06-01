#pragma once
#include <string>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soloud.h>
#include "Events.h"
#include "engine/Context.h"
#include "box2d/box2d.h"
#include <entt/entity/registry.hpp>


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
        [[nodiscard]] float getDeltaTime() const { return deltaTime; };
        entt::registry& getRegistry() { return registry_; };
        [[nodiscard]] entt::entity getPlayer() const { return player_; }
        [[nodiscard]] float getCurrentBPM() const { return bpm_; };
        [[nodiscard]] float getLevelSpeed() const { return current_level_speed_; };

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

        SoLoud::Soloud audio;
        float deltaTime = 1.0f / 60;

        b2WorldId physics_world_;

        physics::PhysicsSystem* physics_system_;
        rendering::RenderingSystem* rendering_system_;
        ui::UISystem* ui_system_;

        entt::registry registry_;
        entt::entity player_;
        float initial_player_position_x_ = -2.0f;

        float velocity_multiplier_ = -2.f;
        float bpm_ = 0.0f; // Default BPM, updated dynamically
        float seconds_per_beat_ = 1.f; //Default, updated according to level music
        float current_level_speed_ = velocity_multiplier_ / seconds_per_beat_;
        float level_length_ = 0; //Default, gets updated according to level music

    private:
        float lastFrameTime_ = 1.0f / 60;
    };
} // gl3

