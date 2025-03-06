#pragma once
#include <string>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace gl3::engine::context {
    class Context {
    public:
        using Callback = std::function<void(Context&)>;

        explicit Context(int width = 800, int height = 600, const std::string &title = "Game");
        virtual ~Context();
        void run(const Callback& update);
        [[nodiscard]] GLFWwindow *getWindow() { return window; }

    private:
        GLFWwindow *window = nullptr;
    };
}
