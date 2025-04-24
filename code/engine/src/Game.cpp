#include "engine/Game.h"
#include <stdexcept>


namespace gl3::engine
{
    using Context = context::Context;

    void Game::run()
    {
        onStartup.invoke(*this);
        start();
        onAfterStartup.invoke(*this);
        context.run([&](Context& ctx)
        {
            onBeforeUpdate.invoke(*this);
            update(getWindow());
            updatePhysics();
            draw();
            renderUI();
            updateDeltaTime();
            onAfterUpdate.invoke(*this);
        });
        onBeforeShutdown.invoke(*this);
        onShutdown.invoke(*this);
    }

    Game::Game(const int width, const int height, const std::string& title, const glm::vec3 camPos,
               const float camZoom): context(width, height, title, camPos, camZoom), physicsWorld(b2_nullWorldId), player(entt::null)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }

        audio.init();
        audio.setGlobalVolume(0.1f);

        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        // We use worldDef to define our physics world
        worldDef.gravity = b2Vec2{0.f, -9.81f};
        physicsWorld = b2CreateWorld(&worldDef);

        initUI();
    }

    void Game::initUI()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imgui_io = &ImGui::GetIO();
        (void)imgui_io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(context.getWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void Game::renderUI()
    {
        // Start the frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        setUpUI(); //set up custom UI layouts in derived game class

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Game::updateDeltaTime()
    {
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime_;
        lastFrameTime_ = frameTime;
    }

    Game::~Game()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
    }
} // gl3
