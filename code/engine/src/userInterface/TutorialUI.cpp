#include "engine/userInterface/TutorialUI.h"

namespace gl3::engine::ui {
    void TutorialUI::DrawHints(const ImGuiViewport* viewport, ImFont* font)
    {
        if (!show_hints_)return; //TODO GameState? lvl? timer
        const auto viewportSize = viewport->Size;
        const auto viewportPos = viewport->Pos;
        ImGui::SetNextWindowPos({viewportPos.x , viewportPos.y});
        ImGui::SetNextWindowSize({viewportSize.x, viewportSize.y});
        ImGui::PushFont(font);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.f, 0.f, 0.f, 0.f});
        ImGui::Begin("Hints", nullptr, flags_);
        const auto windowSize = ImGui::GetWindowSize();
        ImGui::GetStyle().WindowPadding = ImVec2(windowSize.x * 0.08, windowSize.y * 0.08);
        ImGui::GetStyle().ItemSpacing = ImVec2(40, 40);
        ImGui::Text("Press Space to Jump & Escape for Menu");

        ImGui::PopStyleColor();
        ImGui::PopFont();
        ImGui::End();
    }

    void TutorialUI::update()
    {
        DrawHints(ImGui::GetMainViewport(), FontManager::getFont("PixeloidSans"));
    }

} // gl3