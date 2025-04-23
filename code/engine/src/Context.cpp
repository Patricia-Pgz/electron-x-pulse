#include <stdexcept>
#include "engine/Context.h"

#include "../../game/src/Constants.h"
#include "engine/Game.h"


namespace gl3::engine::context
{
    void Context::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        auto contextInstance = static_cast<Context*>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, width, height);
        contextInstance->calculateWindowBounds();
        contextInstance->onFrameBufferSizeChange.invoke();
    }

    void Context::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto contextInstance = static_cast<Context*>(glfwGetWindowUserPointer(window));
        contextInstance->onScrolling.invoke(yoffset);
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
        if (glGetError() != GL_NO_ERROR)
        {
            throw std::runtime_error("gl error");
        }
        calculateWindowBounds();
    }

    void Context::run(const Callback& update)
    {
        glfwSetTime(1.0 / 60);
        while (!glfwWindowShouldClose(window))
        {
            glClearColor(1.f,1.f,1.f, 1.f);
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

        float halfWidth = (static_cast<float>(width) / 2.0f) / pixelsPerMeter;
        float halfHeight = (static_cast<float>(height) / 2.0f) / pixelsPerMeter;

        windowLeft = cameraPosition.x - halfWidth;
        windowRight = cameraPosition.x + halfWidth;
        windowBottom = cameraPosition.y - halfHeight;
        windowTop = cameraPosition.y + halfHeight;
    }

    bool Context::isInVisibleWindow(const glm::vec2& position, const float margin) const
    {
        return position.x >= (windowLeft - margin) &&
            position.x <= (windowRight + margin) &&
            position.y >= (windowBottom - margin) &&
            position.y <= (windowTop + margin);
    }

    Context::~Context()
    {
        glfwTerminate();
    }
}
