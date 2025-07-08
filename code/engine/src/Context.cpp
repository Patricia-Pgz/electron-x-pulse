#include <stdexcept>
#include "engine/Context.h"

#include <GL/gl.h>

#include "engine/Constants.h"
#include "engine/Game.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/rendering/MVPMatrixHelper.h"


namespace gl3::engine::context
{
    void Context::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        auto contextInstance = static_cast<Context*>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, width, height);
        contextInstance->calculateWindowBounds();
        ecs::EventDispatcher::dispatcher.trigger(WindowResizeEvent{width, height});
    }

    void Context::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto contextInstance = static_cast<Context*>(glfwGetWindowUserPointer(window));
        ecs::EventDispatcher::dispatcher.trigger(MouseScrollEvent{xoffset, yoffset});
        contextInstance->calculateWindowBounds(); //recalculate window bounds if user scrolled in the scene
    }

    Context::Context(const int width, const int height, const std::string& title, const glm::vec3 camPos,
                     const float camZoom) : zoom(camZoom), cameraPosition(camPos)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (glGetError() != GL_NO_ERROR)
        {
            throw std::runtime_error("gl error");
        }
        calculateWindowBounds();
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

    void Context::calculateWindowBounds()
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float halfWidth = (width / 2.0f) / pixelsPerMeter / zoom;
        float halfHeight = (height / 2.0f) / pixelsPerMeter / zoom;

        windowLeft = cameraPosition.x - halfWidth;
        windowRight = cameraPosition.x + halfWidth;
        windowBottom = cameraPosition.y - halfHeight;
        windowTop = cameraPosition.y + halfHeight;
    }


    bool Context::isInVisibleWindow(const glm::vec2& position, const glm::vec2 scale, const float margin) const
    {
        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);
        auto fWidth = static_cast<float>(width);
        const auto fHeight = static_cast<float>(height);

        auto screenPos = rendering::MVPMatrixHelper::toScreen(*this, position.x, position.y);
        auto posLeft = position.x - scale.x * 0.5f;
        auto posRight = position.x + scale.x * 0.5f;
        auto posTop = position.y + scale.y * 0.5f;
        auto posBot = position.y - scale.y * 0.5f;

        auto screenPosLeft = rendering::MVPMatrixHelper::toScreen(*this, posLeft, 0.f);
        auto screenPosRight = rendering::MVPMatrixHelper::toScreen(*this, posRight, 0.f);
        auto screenPosTop = rendering::MVPMatrixHelper::toScreen(*this, 0.f, posTop);
        auto screenPosBot = rendering::MVPMatrixHelper::toScreen(*this, 0.f, posBot);

        /*auto x0 = rendering::MVPMatrixHelper::screenToWorld(*this, -1.0, 0.f);
        auto x1 = rendering::MVPMatrixHelper::screenToWorld(*this, 1.0, 0.f);
        x0 = rendering::MVPMatrixHelper::toScreen(*this, x0.x, 0.f);
        x1 = rendering::MVPMatrixHelper::toScreen(*this, x1.x, 0.f);
        fWidth = x1.x - x0.x;*/
        auto screenCamPos = rendering::MVPMatrixHelper::screenToWorld(*this, cameraPosition.x,
                                                                      cameraPosition.y);
        if (screenCamPos.x >= screenPosLeft.x - fWidth && screenCamPos.x <= screenPosRight.x)
        {
            std::cout << "Element within" << std::endl;
        }
        else
        {
            std::cout << "Element out" << std::endl;
        }
        return screenCamPos.x >= screenPosLeft.x - fWidth && screenCamPos.x <= screenPosRight.x;
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
