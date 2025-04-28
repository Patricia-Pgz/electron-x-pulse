#pragma once
#include "engine/ecs/System.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
namespace gl3::engine::ui
{
    class UISystem : public ecs::System
    {
    public:
        explicit UISystem(Game& game) : System(game)
        {
            initUI();
        };
        ~UISystem() override
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
        void initUI()
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            imgui_io = &ImGui::GetIO();
            (void)imgui_io;
            ImGui::StyleColorsDark();

            ImGui_ImplGlfw_InitForOpenGL(game.getWindow(), true);
            ImGui_ImplOpenGL3_Init("#version 460");
        }
        virtual void updateUI(){};

        void renderUI()
        {
            // Start the frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            updateUI(); //setup custom imgui UI layouts in subclass

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    protected:
        ImGuiIO* imgui_io = nullptr;

    };
} // gl3