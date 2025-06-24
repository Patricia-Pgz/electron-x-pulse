#pragma once
#include <string>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace gl3::engine::context
{
    struct WindowResizeEvent
    {
        int newWidth;
        int newHeight;
    };

    struct onMouseScrollEvent
    {
        double xOffset;
        double yOffset;
    };

    class Context
    {
    public:
        using Callback = std::function<void(Context&)>;
        explicit Context(int width = 800, int height = 600, const std::string& title = "Game",
                         glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f), float camZoom = 1.0f);
        virtual ~Context();
        void run(const Callback& update);
        void calculateWindowBounds();
        void setCameraPos(const glm::vec3 position) { cameraPosition = position; }
        void setCameraCenter(const glm::vec3 position) { cameraCenter = position; }
        [[nodiscard]] GLFWwindow* getWindow() const { return window; }
        [[nodiscard]] glm::vec3 getCameraPos() const { return cameraPosition; }
        [[nodiscard]] glm::vec3 getCameraCenter() const { return cameraCenter; }
        [[nodiscard]] float getCurrentZoom() const { return zoom; }
        void setClearColor(const glm::vec4& color) { clearColor = color; }

        [[nodiscard]] std::vector<float> getWindowBounds() const
        {
            return {windowLeft, windowRight, windowBottom, windowTop};
        };
        [[nodiscard]] bool isInVisibleWindow(const glm::vec2& position, float margin = 1.f) const;

    private:
        void onExitApplication() const;
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        GLFWwindow* window = nullptr;
        float zoom;
        glm::vec3 cameraPosition;
        glm::vec3 cameraCenter{0.0f, 0.0f, 0.0f};
        glm::vec4 clearColor = {1, 1, 1, 1};
        float windowLeft, windowRight, windowBottom, windowTop;
    };
}
