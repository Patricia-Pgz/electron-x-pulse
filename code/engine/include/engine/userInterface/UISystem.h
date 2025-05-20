#pragma once
#include <iostream>
#include "engine/ecs/System.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "engine/Assets.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/Game.h"

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
            loadAllFonts(resolveAssetPath("fonts"), 22);
            imgui_io->Fonts->Build();

            ImGui_ImplGlfw_InitForOpenGL(game.getWindow(), true);
            ImGui_ImplOpenGL3_Init("#version 460");
        }

        virtual void updateUI()
        {
        };

        void loadAllFonts(const std::string& fontFolder, const float fontSize)
        {
            imgui_io->Fonts->AddFontDefault();

            for (const auto& entry : std::filesystem::directory_iterator(fontFolder))
            {
                if (entry.is_regular_file())
                {
                    std::string path = entry.path().string();
                    std::string filename = entry.path().filename().string();

                    ImFont* font = imgui_io->Fonts->AddFontFromFileTTF(path.c_str(), fontSize);
                    if (font != nullptr)
                    {
                        loadedFonts[filename] = font;
                    }
                    else
                    {
                        std::cerr << "Failed to load font: " << filename << std::endl;
                    }
                }
            }
        };

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
        std::unordered_map<std::string, ImFont*> loadedFonts;
    };
} // gl3
