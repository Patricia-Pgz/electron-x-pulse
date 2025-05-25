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
            loadAllFonts(resolveAssetPath("fonts"));
            imgui_io->Fonts->Build();

            ImGui_ImplGlfw_InitForOpenGL(game.getWindow(), true);
            ImGui_ImplOpenGL3_Init("#version 460");
        }

        virtual void updateUI()
        {
        };

        void loadFontWithSize(const std::filesystem::path& fontPath, const float fontSize = 22)
        {
            if (!exists(fontPath) || !is_regular_file(fontPath) || fontPath.extension().string() != ".ttf")
            {
                std::cerr << "Font path is invalid: " << fontPath << std::endl;
                return;
            }

            const std::string filename = fontPath.stem().string();
            const std::string key = fontSize == 22
                                        ? filename
                                        : filename + "_" + std::to_string(static_cast<int>(fontSize));

            ImFont* font = imgui_io->Fonts->AddFontFromFileTTF(fontPath.string().c_str(), fontSize);
            if (font != nullptr)
            {
                loadedFonts[key] = font;
            }
            else
            {
                std::cerr << "Failed to load font: " << filename << std::endl;
            }
        }

        void loadAllFonts(const std::string& fontFolder)
        {
            imgui_io->Fonts->AddFontDefault();

            for (const auto& entry : std::filesystem::directory_iterator(fontFolder))
            {
                if (entry.is_regular_file())
                {
                    loadFontWithSize(entry.path());
                }
            }

            loadFontWithSize(fontFolder + "/PixeloidSans-Bold.ttf", 26);
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
