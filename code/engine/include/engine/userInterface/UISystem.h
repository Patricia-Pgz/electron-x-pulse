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
    ///@note Only add one high level UI System per game.
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

        [[nodiscard]] ImGuiIO* getImGuiIO() const
        {
            return imgui_io;
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
        }

        void renderUI()
        {
            // Start the frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (!pendingSubsystems.empty() && !isInitializingSystems)
            {
                initializeSubsystems();
            }

            updateUI();
            updateSubSystems();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        virtual void updateUI()
        {
        };

        /**
        * @brief Register UI Systems (IUISubsystem) that should be updated each UI Frame.
        * @note Use this method in your game in @ref registerUiSystems() in order to register your systems on time. They won't be included otherwise.
        * The subsystems will be initialized, once the ImGui Frame is ready.
        */
        template <typename T>
        void registerSubsystem()
        {
            static_assert(std::is_base_of_v<IUISubsystem, T>, "T must derive from IUISubsystem");
            pendingSubsystems.emplace_back([this]
            {
                return std::make_unique<T>(imgui_io, game);
            });
        }

        /**
        * @brief Initializes the pre-registered UI subsystems.
        */
        void initializeSubsystems()
        {
            if (!(ImGui::GetCurrentContext() && ImGui::GetCurrentContext()->WithinFrameScope))
            {
                std::cerr << "ImGui not initialized or not in frame scope" << std::endl;
                return;
            }
            isInitializingSystems = true;
            for (auto& factory : pendingSubsystems)
            {
                subsystems.emplace_back(factory());
            }

            pendingSubsystems.clear();
            isInitializingSystems = false;
        }

    protected:
        ImGuiIO* imgui_io = nullptr;
        std::vector<std::function<std::unique_ptr<IUISubsystem>()>> pendingSubsystems;
        std::vector<std::unique_ptr<IUISubsystem>> subsystems;
        bool isInitializingSystems = false;

        void updateSubSystems() const
        {
            if (!(ImGui::GetCurrentContext() && ImGui::GetCurrentContext()->WithinFrameScope))
            {
                std::cerr << "Not inside an ImGui Frame or Context" << std::endl;
                return;
            }
            if (subsystems.empty()) return;

            for (auto& system : subsystems)
            {
                system->update();
            }
        }
    };
} // gl3
