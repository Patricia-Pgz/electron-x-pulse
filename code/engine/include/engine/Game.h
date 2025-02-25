#pragma once
#include <string>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soloud.h>
#include "box2d/box2d.h"


namespace gl3::engine {

class Game {
public:
    glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);
    GLFWwindow *getWindow() const { return window; }
    b2WorldId getPhysicsWorld() const;

protected:
    Game(int width, int height, const std::string &title, glm::vec3 camPos, float camZoom);
    virtual ~Game();
    virtual void run(){};
    virtual void update(){};
    virtual void draw(){};
    void updateDeltaTime();

    GLFWwindow *window = nullptr;
    void calculateWindowBounds();
    float windowLeft, windowRight, windowBottom, windowTop;

    SoLoud::Soloud audio;
    float deltaTime = 1.0f / 60;

    glm::vec3 cameraPosition;
    glm::vec3 cameraCenter {0.0f, 0.0f, 0.0f};
    float zoom;

    b2WorldId physicsWorld;



private:
    float lastFrameTime = 1.0f / 60;

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);



};

} // gl3

