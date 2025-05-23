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

    namespace levelLoading
    {
        class LevelUISystem;
    }

    namespace editor
    {
        class EditorSystem;
        class EditorUISystem;
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
        [[nodiscard]] GLFWwindow* getWindow() const { return context.getWindow(); }
        [[nodiscard]] b2WorldId getPhysicsWorld() const { return physics_world; };
        [[nodiscard]] const context::Context& getContext() const { return context; }
        [[nodiscard]] float getDeltaTime() const { return deltaTime; };
        entt::registry& getRegistry() { return registry_; };
        [[nodiscard]] entt::entity getPlayer() const { return player; }
        [[nodiscard]] float getCurrentBPM() const { return bpm; };
        [[nodiscard]] float getLevelSpeed() const { return currentLevelSpeed; };


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

        virtual void updateUI();

        context::Context context;

        SoLoud::Soloud audio;
        float deltaTime = 1.0f / 60;

        b2WorldId physics_world;
        physics::PhysicsSystem* physics_system;
        rendering::RenderingSystem* rendering_system;
        levelLoading::LevelUISystem* lvl_ui_system;

        entt::registry registry_;
        entt::entity player;

        float initialPlayerPositionX = -2.0f;

        float velocityMultiplier = -2.f;
        float bpm = 0.0f; // Default BPM, updated dynamically
        float secondsPerBeat = 1.f; //Default, updated according to level music
        float currentLevelSpeed = velocityMultiplier / secondsPerBeat;
        float levelLength = 0; //Default, gets updated according to level music

    private:
        float lastFrameTime_ = 1.0f / 60;
    };
} // gl3

