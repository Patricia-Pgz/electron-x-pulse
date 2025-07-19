#pragma once
#include <iostream>
#include <typeindex>
#include <ranges>
#include "engine/ecs/System.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "engine/Assets.h"
#include "engine/Game.h"
#include "engine/userInterface/FontManager.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::engine::ui
{
    /**
     * @class UISystem
     * @brief High level UI system for Game. Can register multiple custom IUISubsystem
     *
     * Initializes ImGui, loads fonts, and updates itself as well as all registered UI subsystems
     * each UI frame.
     *
     * @note Only one high-level UI system should be existing per game. This is already added to Game.
     */
    class UISystem final : public ecs::System
    {
    public:
        using event_t = events::Event<UISystem>;
        event_t onInitialized;

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

        void renderUI(const float deltaTime)
        {
            if (!is_active) { return; }
            // Start the frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (!pendingSubsystems.empty() && !isInitializingSystems)
            {
                initializeSubsystems();
                onInitialized.invoke();
            }

            updateSubSystems(deltaTime); //update Subsystems inside frame

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        template <typename T>
        T* getSubsystem() const
        {
            static_assert(std::is_base_of_v<IUISubsystem, T>, "T must derive from IUISubsystem");
            if (const auto subSys = subsystems.find(typeid(T)); subSys != subsystems.end())
            {
                return dynamic_cast<T*>(subSys->second.get());
            }
            return nullptr;
        }

        /**
        * @brief Register UI Systems (IUISubsystem) that should be updated each UI Frame.
        * @note Use this method in your game in @ref registerUiSystems() in order to register your systems on time. They won't be included otherwise.
        * The subsystems will be initialized, once the ImGui Frame is ready.
        */
        template <typename T>
        void registerSubsystem()
        {
            static_assert(std::is_base_of_v<IUISubsystem, T>, "T must derive from IUISubsystem");
            pendingSubsystems.emplace_back(
                typeid(T),
                [this]()
                {
                    return std::make_unique<T>(imgui_io, game);
                }
            );
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

            for (auto& [type, factory] : pendingSubsystems)
            {
                subsystems[type] = factory();
            }

            pendingSubsystems.clear();
            isInitializingSystems = false;
        }

    private:
        /// ImGui IO pointer.
        ImGuiIO* imgui_io = nullptr;

        /// List of subsystems waiting to be initialized.
        std::vector<std::pair<std::type_index, std::function<std::unique_ptr<IUISubsystem>()>>> pendingSubsystems;

        /// Map of initialized subsystems.
        std::unordered_map<std::type_index, std::unique_ptr<IUISubsystem>> subsystems;

        /// Whether subsystems are currently being initialized.
        bool isInitializingSystems = false;

        /**
         * @brief Update all active UI subsystems.
         * @note Called each UI frame during @ref renderUI.
         */
        void updateSubSystems(const float deltaTime) const
        {
            if (!(ImGui::GetCurrentContext() && ImGui::GetCurrentContext()->WithinFrameScope))
            {
                std::cerr << "Not inside an ImGui Frame or Context" << std::endl;
                return;
            }
            if (subsystems.empty()) return;

            for (const auto& sys : subsystems | std::views::values)
            {
                if (!sys->isActive()) continue;
                sys->update(deltaTime);
            }
        }
    };
} // gl3
