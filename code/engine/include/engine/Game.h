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


namespace gl3::engine {

class Game {
public:
    void run();
    glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);
    GLFWwindow *getWindow() { return context.getWindow(); }
    b2WorldId getPhysicsWorld() const;
    void calculateWindowBounds();

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

    float windowLeft, windowRight, windowBottom, windowTop; //TODO context

    SoLoud::Soloud audio;
    float deltaTime = 1.0f / 60;

    glm::vec3 cameraPosition;
    glm::vec3 cameraCenter {0.0f, 0.0f, 0.0f};
    float zoom;

    b2WorldId physicsWorld;

private:
    context::Context context;
    float lastFrameTime = 1.0f / 60;
};

} // gl3

