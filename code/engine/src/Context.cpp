/**
* @file Context.cpp
 * @brief Implementation of the core OpenGL context and window management.
 */
#include <stdexcept>
#include "engine/Context.h"
#include "engine/Constants.h"
#include "engine/Game.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/rendering/MVPMatrixHelper.h"

namespace gl3::engine::context
{
    void Context::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        const auto contextInstance = static_cast<Context*>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, width, height);
        contextInstance->calculateWorldWindowBounds();
    }

    void Context::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        ecs::EventDispatcher::dispatcher.trigger(MouseScrollEvent{xoffset, yoffset});
    }

    Context::Context(const int width, const int height, const std::string& title, const glm::vec3 camPos,
                     const float camZoom) : zoom(camZoom), cameraPosition(camPos)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }

        // Request OpenGL 4.6 Core profile.
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        // Color buffer bit depth.
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr)
        {
            throw std::runtime_error("Failed to create window");
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetScrollCallback(window, scroll_callback);
        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (glGetError() != GL_NO_ERROR)
        {
            throw std::runtime_error("gl error");
        }
        calculateWorldWindowBounds();
        ecs::EventDispatcher::dispatcher.sink<ecs::GameExit>().connect<&
            Context::onExitApplication>(this);
    }

    void Context::run(const Callback& update)
    {
        glfwSetTime(1.0 / 60);
        while (!glfwWindowShouldClose(window))
        {
            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            update(*this);
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
    }

    void Context::setCameraPosAndCenter(glm::vec3 position, glm::vec3 center)
    {
        cameraPosition = position;
        cameraCenter = center;
        calculateWorldWindowBounds();
    }

    void Context::moveCameraX(const float dx)
    {
        cameraPosition.x += dx * 1.0f;
        cameraCenter.x += dx * 1.0f;
        calculateWorldWindowBounds();
    }


    void Context::calculateWorldWindowBounds()
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        const auto fWidth = static_cast<float>(width);
        const auto fHeight = static_cast<float>(height);
        const auto BottomRightScreenCorner = rendering::MVPMatrixHelper::screenToWorld(*this, fWidth, fHeight);


        auto windowLeftWorld = BottomRightScreenCorner.x - fWidth / pixelsPerMeter;
        auto windowRightWorld = BottomRightScreenCorner.x;
        auto windowTopWorld = BottomRightScreenCorner.y + fHeight / pixelsPerMeter;
        auto windowBottomWorld = BottomRightScreenCorner.y;

        windowBounds = {windowLeftWorld, windowRightWorld, windowTopWorld, windowBottomWorld};

        ecs::EventDispatcher::dispatcher.trigger(WindowBoundsRecomputeEvent{
            width, height, &windowBounds
        });
    }

    bool Context::isInVisibleWindow(const glm::vec2& position, const glm::vec2 scale, const float margin) const
    {
        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);
        const auto fWidth = static_cast<float>(width);
        const auto fHeight = static_cast<float>(height);
        const auto posLeft = position.x - scale.x * 0.5f;
        const auto posRight = position.x + scale.x * 0.5f;

        const auto worldBottomRightScreenCorner = rendering::MVPMatrixHelper::screenToWorld(*this, fWidth, fHeight);
        const auto worldScreenWidth = fWidth / pixelsPerMeter;

        return posRight >= worldBottomRightScreenCorner.x - worldScreenWidth - margin && posLeft <=
            worldBottomRightScreenCorner.x + margin;
    }

    void Context::onExitApplication() const
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    Context::~Context()
    {
        ecs::EventDispatcher::dispatcher.sink<ecs::GameExit>().disconnect<&
            Context::onExitApplication>(this);
        glfwTerminate();
    }
}
