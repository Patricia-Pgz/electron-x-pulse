#include "FinishUI.h"
#include "UIEvents.h"
#include "engine/userInterface/FontManager.h"
#include "engine/userInterface/UIConstants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/rendering/TextureManager.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::game::ui
{
    void FinishUI::styleWindow(const ImVec2 windowSize)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(windowSize.x * 0.08f, windowSize.y * 0.14f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(40, 40));

        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, UINeonColors::Cyan);
    }

    void FinishUI::DrawFinishScreen(const ImGuiViewport* viewport, ImFont* heading, ImFont* font)
    {
        const auto viewportSize = viewport->Size;
        const auto viewportPos = viewport->Pos;
        const ImVec2 windowSize = {viewportSize.x * 0.5f, viewportSize.x * 0.5f};
        ImGui::SetNextWindowSize({windowSize.x, windowSize.y});
        ImGui::SetNextWindowPos({(viewportSize.x - windowSize.x) * 0.5f, viewportPos.y});
        ImGui::PushFont(heading);

        ImGui::Begin("Menu", nullptr, flags);
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
            engine::ecs::EventDispatcher::dispatcher.trigger(events::ShowFinishScreen{false});
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
            engine::rendering::TextureManager::getUITexture("Win")->getID(),
            windowPos,
            ImVec2(windowPos.x + windowSizeCur.x,
                   windowPos.y + windowSizeCur.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        ImGui::PopStyleVar(5);
        ImGui::PopStyleColor(8);
        ImGui::PopFont();
        ImGui::End();
    }


    /**
     * @brief Draws the Victory screen. (update only gets called, if is_active)
 */
    void FinishUI::update(const float deltaTime)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        DrawFinishScreen(viewport, engine::ui::FontManager::getFont("pixeloid-bold-30"),
                         engine::ui::FontManager::getFont("pixeloid-bold-26"));
    }
}
