#include "engine/userInterface/InGameMenuSystem.h"

#include <imgui_internal.h>
#include <iostream>
#include <engine/userInterface/FontManager.h>

#include "engine/Constants.h"
#include "engine/rendering/TextureManager.h"

namespace gl3::engine::inGameUI
{
    void styleWindow(const ImVec2 windowSize)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.f;
        style.WindowPadding = ImVec2(windowSize.x * 0.08, windowSize.y * 0.14);
        ImGui::GetStyle().FrameRounding = 5.0;

        style.ItemSpacing = ImVec2(20, 20);
    }

    void InGameMenuSystem::DrawInGameUI(const ImGuiViewport* viewport, ImFont* font)
    {
        const auto viewportSize = viewport->Size;
        const auto viewportPos = viewport->Pos;
        ImGui::SetNextWindowPos({viewportPos.x * 0.5f, viewportPos.y});
        ImGui::SetNextWindowSize({viewportSize.x * 0.5f, viewportSize.y});
        ImGui::PushFont(font);
        const auto windowSize = ImGui::GetWindowSize();
        const auto windowPos = ImGui::GetWindowPos();

        styleWindow(windowSize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, UINeonColors::softPastelPink);
        ImGui::Begin("Menu", nullptr, flags_);
        ImGui::PopStyleColor();

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::getUITexture("LvlSelectBG1").getID(),
            windowPos,
            ImVec2(windowPos.x + windowSize.x,
                   windowPos.y + windowSize.y)
        );

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::getUITexture("LvlSelectBGTop1").getID(),
            windowPos,
            ImVec2(windowPos.x + windowSize.x,
                   windowPos.y + windowSize.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        ImGui::PopFont();
        ImGui::End();
    }


    /**
 * @brief Shows or hides the in-game menu.
 * @pre ImGui needs to be set up already. E.g. by calling @ref gl3::engine::ui::UISystem::renderUI on a UISystem instance (or inherited ones) each game UI-Update frame @ref gl3::engine::Game::updateUI.
 * @pre ImGui Frame needs to be running already.
 */
    void InGameMenuSystem::update()
    {
        std::cout << "update!";
        if (!(ImGui::GetCurrentContext() && ImGui::GetCurrentContext()->WithinFrameScope))
        {
            std::cerr << "Not inside an ImGui Frame or Context" << std::endl;
            return;
        }
        if (glfwGetKey(game_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && is_collapsed_)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            DrawInGameUI(viewport, ui::FontManager::getFont("PixeloidSans-Bold_26"));
        }
    }
}
