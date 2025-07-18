#include "InstructionUI.h"
#include "engine/userInterface/FontManager.h"

namespace gl3::game::ui
{
    void InstructionUI::drawHints(const ImGuiViewport* viewport, ImFont* font)
    {
        if (!pause_timer_) { timer_ -= game.getDeltaTime(); }

        const auto viewportSize = viewport->Size;
        const auto viewportPos = viewport->Pos;
        ImGui::SetNextWindowPos({viewportPos.x, viewportPos.y});
        ImGui::SetNextWindowSize({viewportSize.x * 0.8f, viewportPos.y + viewportSize.y * 0.2f});
        ImGui::PushFont(font);
        ImGui::Begin("Hints", nullptr, flags_);
        const auto windowSize = ImGui::GetWindowSize();
        ImGui::GetStyle().WindowPadding = ImVec2(windowSize.x * 0.08f, windowSize.y * 0.08f);
        ImGui::GetStyle().ItemSpacing = ImVec2(40, 40);
        const std::string text1 = "Press SPACE to Jump & ESC to Open/Close Menu";
        const std::string text = edit_mode_ ? text1 + " & ENTER to Start/Reset Play-Mode" : text1;
        ImGui::Text(text.c_str());

        ImGui::PopFont();
        ImGui::End();
    }

    void InstructionUI::onRestartLevel() {
        if(!is_active)return;
        resetTimer();
    }


    void InstructionUI::update()
    {
        if (timer_ <= 0.f) return;
        drawHints(ImGui::GetMainViewport(), engine::ui::FontManager::getFont("PixeloidSans"));
    }

    void InstructionUI::setActive(const bool setActive)
    {
        is_active = setActive;
        if(!setActive) return;
        resetTimer();
    }

    void InstructionUI::resetTimer()
    {
        timer_ = 15;
        pause_timer_ = false;
    }
} // gl3
