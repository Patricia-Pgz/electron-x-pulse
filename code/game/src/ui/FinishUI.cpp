#include "FinishUI.h"
#include "engine/userInterface/FontManager.h"
#include "engine/Constants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/rendering/TextureManager.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::game::ui
{
    void FinishUI::styleWindow(const ImVec2 windowSize)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.f;
        style.WindowPadding = ImVec2(windowSize.x * 0.08, windowSize.y * 0.14);

        style.FrameRounding = 5.0;
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

    void FinishUI::DrawFinishScreen(const ImGuiViewport* viewport, ImFont* heading, ImFont* font)
    {
        const auto viewportSize = viewport->Size;
        const auto viewportPos = viewport->Pos;
        const ImVec2 windowSize = {viewportSize.x * 0.5f, viewportSize.x * 0.5f};
        ImGui::SetNextWindowSize({windowSize.x, windowSize.y});
        ImGui::SetNextWindowPos({(viewportSize.x - windowSize.x) * 0.5f, viewportPos.y});
        ImGui::PushFont(heading);

        ImGui::Begin("Menu", nullptr, flags_);
        const auto windowSizeCur = ImGui::GetWindowSize();
        const auto windowPos = ImGui::GetWindowPos();
        styleWindow(windowSizeCur);

        const ImVec2 textSize = ImGui::CalcTextSize("Victory!");
        ImGui::SetCursorPos({(windowSizeCur.x - textSize.x) * 0.5f, windowPos.y + windowSizeCur.y * 0.35f});
        ImGui::Text("Victory!");
        ImGui::PopFont();
        ImGui::PushFont(font);
        const ImVec2 padding = ImGui::GetStyle().FramePadding;
        const ImVec2 lvlSelectSize = ImGui::CalcTextSize("Level Selection");
        ImGui::SetCursorPosX((windowSizeCur.x - lvlSelectSize.x - 2 * padding.x) * 0.5f);
        if (ImGui::Button("Restart Level", {lvlSelectSize.x + 2 * padding.x, lvlSelectSize.y + 2 * padding.y}))
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(engine::ui::RestartLevelEvent{true});
        }

        ImGui::SetCursorPosX((windowSizeCur.x - lvlSelectSize.x - 2 * padding.x) * 0.5f);
        if (ImGui::Button("Level Selection", {lvlSelectSize.x + 2 * padding.x, lvlSelectSize.y + 2 * padding.y}))
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::GameStateChange{
                engine::GameState::LevelSelect
            });
        }

        ImGui::GetWindowDrawList()->AddImage(
            engine::rendering::TextureManager::getUITexture("Win").getID(),
            windowPos,
            ImVec2(windowPos.x + windowSizeCur.x,
                   windowPos.y + windowSizeCur.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        ImGui::PopStyleColor(8);
        ImGui::PopFont();
        ImGui::End();
    }


    /**
     * @brief Shows or hides the in-game menu.
     * @pre ImGui needs to be set up already. E.g. by calling @ref gl3::engine::ui::UISystem::renderUI on a UISystem instance (or inherited ones) each game UI-Update frame @ref gl3::engine::Game::updateUI.
    * @pre ImGui Frame needs to be running already.
 */
    void FinishUI::update()
    {
        if (!is_active) return;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        DrawFinishScreen(viewport, engine::ui::FontManager::getFont("pixeloid-bold-30"),
                         engine::ui::FontManager::getFont("pixeloid-bold-26"));
    }
}
