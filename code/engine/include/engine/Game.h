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

class Game {
public:
    void run();
    [[nodiscard]] glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale) const;
    [[nodiscard]] GLFWwindow *getWindow() const { return context.getWindow(); }
    [[nodiscard]] b2WorldId getPhysicsWorld() const;

    using event_t = events::Event<Game, Game&>;

    event_t onStartup;
    event_t onAfterStartup;
    event_t onBeforeUpdate;
    event_t onAfterUpdate;
    event_t onBeforeShutdown;
    event_t onShutdown;

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

private:
    float lastFrameTime_ = 1.0f / 60;
    entt::registry registry_;
};

} // gl3

