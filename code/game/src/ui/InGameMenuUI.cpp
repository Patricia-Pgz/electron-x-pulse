#include "InGameMenuUI.h"
#include "engine/userInterface/FontManager.h"
#include "engine/userInterface/UIConstants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/rendering/TextureManager.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::game::ui
{
    void styleWindow(const ImVec2 windowSize)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.f;
        style.WindowPadding = ImVec2(windowSize.x * 0.08, windowSize.y * 0.14);
        ImGui::GetStyle().FrameRounding = 5.0;
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, UINeonColors::Cyan);

        style.ItemSpacing = ImVec2(40, 40);
        style.FramePadding = ImVec2(10, 10);
    }

    void InGameMenuUI::DrawInGameUI(const ImGuiViewport* viewport, ImFont* font)
    {
        const auto viewportSize = viewport->Size;
        const auto viewportPos = viewport->Pos;
        ImGui::SetNextWindowPos({viewportPos.x, viewportPos.y});
        ImGui::SetNextWindowSize({viewportSize.x, viewportSize.y});
        ImGui::PushFont(font);

        ImGui::Begin("Menu", nullptr, flags_);
        const auto windowSize = ImGui::GetWindowSize();
        const auto windowPos = ImGui::GetWindowPos();
        styleWindow(windowSize);

        ImGui::SetCursorPosY(windowSize.y * 0.3f);

        const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
        const ImVec2 padding = ImGui::GetStyle().FramePadding;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, 20.0f));
        const ImVec2 textSize = ImGui::CalcTextSize("Volume:");
        ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
        ImGui::Text("Volume:");
        ImGui::PopStyleVar();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing.x, spacing.x));
        const auto sliderWidth = textSize.x * 1.7f;
        ImGui::PushItemWidth(sliderWidth);
        ImGui::SetCursorPosX((windowSize.x - sliderWidth) * 0.5f);
        if (ImGui::SliderFloat("##Volume", &volume_, 0.0f, 1.0f, "%.2f"))
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(engine::ui::VolumeChangeEvent(volume_));
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3 * padding.y);
        const ImVec2 lvlSelectSize = ImGui::CalcTextSize("Level Selection");
        ImGui::SetCursorPosX((windowSize.x - lvlSelectSize.x - 2 * padding.x) * 0.5f);
        if (ImGui::Button("Restart Level", {lvlSelectSize.x + 2 * padding.x, lvlSelectSize.y + 2 * padding.y}))
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(engine::ui::RestartLevelEvent{true});
        }

        ImGui::SetCursorPosX((windowSize.x - lvlSelectSize.x - 2 * padding.x) * 0.5f);
        if (ImGui::Button("Level Selection", {lvlSelectSize.x + 2 * padding.x, lvlSelectSize.y + 2 * padding.y}))
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::GameStateChange{
                engine::GameState::LevelSelect
            });
        }


        ImGui::GetWindowDrawList()->AddImage(
            engine::rendering::TextureManager::getUITexture("LvlSelectBGTop1")->getID(),
            windowPos,
            ImVec2(windowPos.x + windowSize.x,
                   windowPos.y + windowSize.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(8);
        ImGui::PopFont();
        ImGui::End();
    }


    /**
     * @brief Shows or hides the in-game menu.
     * @pre ImGui needs to be set up already. E.g. by calling @ref gl3::engine::ui::UISystem::renderUI on a UISystem instance (or inherited ones) each game UI-Update frame @ref gl3::engine::Game::updateUI.
    * @pre ImGui Frame needs to be running already.
 */
    void InGameMenuUI::update()
    {
        if (!is_active) return;
        if (glfwGetKey(game_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            if (!escape_pressed_)
            {
                escape_pressed_ = true;
                show_ui = !show_ui;
                engine::ecs::EventDispatcher::dispatcher.trigger(engine::ui::PauseLevelEvent{show_ui});
            }
        }
        else if (glfwGetKey(game_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        {
            escape_pressed_ = false;
        }

        if (!show_ui) return;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        DrawInGameUI(viewport, engine::ui::FontManager::getFont("pixeloid-bold-26"));
    }
}
