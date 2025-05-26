#pragma once
#include "engine/ecs/System.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "engine/Assets.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/Game.h"
#include "engine/userInterface/FontManager.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::engine::ui
{
    ///summary
    ///Parent class for high level UI system.
    ///Initializes ImGui and updates itself as well as UI subsystems each UI frame.
    ///@note Only add one high level UI per game.
    ///summary
    class UISystem : public ecs::System
    {
    public:
        explicit UISystem(Game& game) : System(game)
        {
        };

        ~UISystem() override
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        /**
        * @brief Init ImGui and ImGui Frame. Load all fonts from font asset folder.
        * @note Only call this once per game (see constructor @ref gl3::engine::Game::Game).
        */
        void initUI()
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            imgui_io = &ImGui::GetIO();
            (void)imgui_io;
            ImGui::StyleColorsDark();
            FontManager::loadFonts(resolveAssetPath("fonts"));
            imgui_io->Fonts->Build();

            ImGui_ImplGlfw_InitForOpenGL(game.getWindow(), true);
            ImGui_ImplOpenGL3_Init("#version 460");
            initSubsystems();
        }

        virtual void initSubsystems(){}

        virtual void updateUI(){};

        void renderUI()
        {
            // Start the frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            updateUI(); //setup custom imgui UI layouts in subclass
            updateSubSystems();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

    protected:
        ImGuiIO* imgui_io = nullptr;
        std::vector<std::unique_ptr<IUISubsystem>> subsystems;

        void addSubsystem(std::unique_ptr<IUISubsystem> subsystem)
        {
            subsystems.emplace_back(std::move(subsystem));
        }

        void updateSubSystems() const
        {
            for (auto& system : subsystems)
            {
                system->update();
            }
        }
    };
} // gl3
