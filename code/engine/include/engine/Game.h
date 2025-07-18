#pragma once
#include <string>
#include <glm/glm.hpp>
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

    namespace audio
    {
        class AudioSystem;
    }

    namespace state
    {
        class StateManagementSystem;
    }

    /**
     * @brief Defines the different states the game can be in.
     */
    enum class GameState
    {
        None,                  ///< No active state.
        LevelSelect,           ///< Selecting a level.
        EditMode,              ///< Editing mode.
        Menu,                  ///< Main menu.
        Level                 ///< Level gameplay.
    };

    /**
     *@class Game
     * @brief The core Game class.
     *
     * Manages all main game systems including physics, rendering, audio, UI, and state management.
     * Handles the game loop, delta time calculation, and player entity.
     */
    class Game
    {
    public:
        /**
         * @brief Repeatedly runs the main game loop.
         */
        void run();

        /// @return The underlying GLFW window.
        [[nodiscard]] GLFWwindow* getWindow() const { return context.getWindow(); }

        /// @return The Box2D physics world.
        [[nodiscard]] b2WorldId getPhysicsWorld() const { return physics_world; };

        /// @return The game rendering context.
        [[nodiscard]] const context::Context& getContext() const { return context; }

        /// @return Mutable reference to the game rendering context.
        context::Context& getContext() { return context; }

        /// @return The time step between frames.
        [[nodiscard]] float getDeltaTime() const { return delta_time; };

        /// @return The EnTT ECS registry.
        entt::registry& getRegistry() { return registry; };

        /// @return The UI system.
        [[nodiscard]] ui::UISystem* getUISystem() const { return ui_system; };

        /// @return The physics system.
        [[nodiscard]] physics::PhysicsSystem* getPhysicsSystem() const { return physics_system; }

        /// @return The audio system.
        [[nodiscard]] audio::AudioSystem* getAudioSystem() const { return audio_system; };

        /// @return The state management system.
        [[nodiscard]] state::StateManagementSystem* getStateManagement() const { return state_management_system; };

        /// @return The player entity.
        [[nodiscard]] entt::entity getPlayer() const { return player; }

        /**
         * @brief Set the player entity.
         * @param player_ The new player entity.
         */
        void setPlayer(const entt::entity player_) { player = player_; }

        /**
         * @brief Track the current level's paused status manually.
         * @param isPaused True to pause, false to unpause.
         */
        void setPaused(const bool isPaused) { is_paused = isPaused; }

        /// @return True if the game is paused.
        [[nodiscard]] bool isPaused() const { return is_paused; }

        /**
         * @brief Core game loop events.
         */
        using event_t = events::Event<Game, Game&>;
        event_t onStartup;         ///< Called when the game starts up.
        event_t onAfterStartup;    ///< Called immediately after startup.
        event_t onBeforeUpdate;    ///< Called before each update.
        event_t onAfterUpdate;     ///< Called after each update.
        event_t onBeforeShutdown;  ///< Called before shutdown.
        event_t onShutdown;        ///< Called when the game shuts down.

    protected:
        /**
         * @brief Construct the Game.
         * @param width Initial window width.
         * @param height Initial window height.
         * @param title Window title.
         * @param camPos Initial camera position.
         * @param camZoom Initial camera zoom level. @note zoom is not handled in Context window bounds calculation, preferably leave it as is or add engine functionality
         */
        Game(int width, int height, const std::string& title, glm::vec3 camPos, float camZoom = 1.f/100.f);

        /**
         * @brief Destroy the Game.
         */
        virtual ~Game();

        /**
         * @brief Calculate the frame's delta time.
         */
        void updateDeltaTime();

        /**
         * @brief Start the game and e.g. call run() to run the game loop repeatedly.
         */
        virtual void start() {}

        /**
         * @brief Called every frame to update game logic.
         * @param window The GLFW window.
         */
        virtual void update(GLFWwindow* window) {}

        /**
         * @brief Called to update the physics simulation.
         */
        virtual void updatePhysics();

        /**
         * @brief Called to draw/render the frame.
         */
        virtual void draw();

        /**
         * @brief Register custom UI subsystems.
         * Called once during startup.
         */
        virtual void registerUiSystems() {}

        /**
         * @brief Called to update the UI every frame.
         */
        virtual void updateUI();

        /**
         * @brief Called to update the StateManagementSystem and AudioSystem.
         */
        virtual void updateState();

        context::Context context; ///< Rendering context.
        float delta_time = 1.0f / 60; ///< Time step between frames.
        b2WorldId physics_world; ///< Box2D physics world.

        physics::PhysicsSystem* physics_system;
        rendering::RenderingSystem* rendering_system;
        ui::UISystem* ui_system;
        audio::AudioSystem* audio_system;
        state::StateManagementSystem* state_management_system;

        entt::registry registry; ///< ECS registry.
        entt::entity player; ///< Player entity.

        bool is_paused = true;

    private:
        float lastFrameTime = 1.0f / 60; ///< Last frame time for delta calculation.
    };
} // namespace gl3::engine
