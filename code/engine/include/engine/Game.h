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



namespace gl3::engine {

    enum class GameState {
        Menu,
        Level,
        PreviewWithScrolling,
        PreviewWithTesting
    };

class Game {
public:
    void run();
    [[nodiscard]] GLFWwindow *getWindow() const { return context.getWindow(); }
    [[nodiscard]] b2WorldId getPhysicsWorld() const {return physicsWorld;};
    [[nodiscard]] const context::Context& getContext() const { return context; }
    [[nodiscard]] float getDeltaTime() const {return deltaTime;};
    entt::registry& getRegistry(){return registry_;};
    float& getLevelSpeed() { return levelSpeed; };
    float& getCurrentBPM(){return bpm;};


    using event_t = events::Event<Game, Game&>;

    event_t onStartup;
    event_t onAfterStartup;
    event_t onBeforeUpdate;
    event_t onAfterUpdate;
    event_t onBeforeShutdown;
    event_t onShutdown;


    GameState currentGameState = GameState::Menu;

protected:
    Game(int width, int height, const std::string &title, glm::vec3 camPos, float camZoom);
    virtual ~Game();
    virtual void start() {};
    virtual void update(GLFWwindow *window) {};
    virtual void updatePhysics(){};
    virtual void draw(){};
    void updateDeltaTime();

    context::Context context;

    SoLoud::Soloud audio;
    float deltaTime = 1.0f / 60;

    b2WorldId physicsWorld;

    entt::registry registry_;

    float levelSpeed = -1.f;
    float bpm = 0.0f; // Default BPM, updated dynamically

private:
    float lastFrameTime_ = 1.0f / 60;
};

} // gl3

