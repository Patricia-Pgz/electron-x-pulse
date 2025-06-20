#include "InstructionUI.h"
#include "engine/userInterface/FontManager.h"

namespace gl3::game::ui
{
    void InstructionUI::drawHints(const ImGuiViewport* viewport, ImFont* font)
    {
        if(!pause_timer){timer_ -= game_.getDeltaTime();}

        const auto viewportSize = viewport->Size;
        const auto viewportPos = viewport->Pos;
        ImGui::SetNextWindowPos({viewportPos.x, viewportPos.y});
        ImGui::SetNextWindowSize({viewportSize.x, viewportSize.y});
        ImGui::PushFont(font);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.f, 0.f, 0.f, 0.f});
        ImGui::Begin("Hints", nullptr, flags_);
        const auto windowSize = ImGui::GetWindowSize();
        ImGui::GetStyle().WindowPadding = ImVec2(windowSize.x * 0.08, windowSize.y * 0.08);
        ImGui::GetStyle().ItemSpacing = ImVec2(40, 40);
        ImGui::Text("Press SPACE to Jump & ESC to open/close Menu");

        ImGui::PopStyleColor();
        ImGui::PopFont();
        ImGui::End();
    }

    void InstructionUI::update()
    {
        if (!is_active || timer_ <= 0.f) return;
        drawHints(ImGui::GetMainViewport(), engine::ui::FontManager::getFont("PixeloidSans"));
    }

    void InstructionUI::setActive(const bool setActive)
    {
        is_active = setActive;
        timer_ = 15;
        pause_timer = false;
    }

} // gl3
