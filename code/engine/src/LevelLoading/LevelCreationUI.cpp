#include "engine/levelLoading/LevelCreationUI.h"

#include <iostream>

#include "../../../game/src/ui/FinishUI.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "engine/userInterface/FontManager.h"

namespace gl3::engine::levelLoading
{
    inline bool DropDown(const char* id, int& currentIndex, const std::vector<std::string>& items)
    {
        // no empty list = no empty options
        if (items.empty())
            return false;

        // Protect index from invalid values
        if (currentIndex < 0 || currentIndex >= static_cast<int>(items.size()))
            currentIndex = 0;

        bool changed = false;

        if (ImGui::BeginCombo(id, items[currentIndex].c_str()))
        {
            for (int i = 0; i < items.size(); i++)
            {
                const bool isSelected = (currentIndex == i);

                if (ImGui::Selectable(items[i].c_str(), isSelected))
                {
                    currentIndex = i;
                    changed = true;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        return changed; // caller can react to changes
    }

    inline void ColorPicker(bool& pickerWasOpen, glm::vec4& color, const char* id)
    {
        ImGui::PushID(id);
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
        ImGui::ColorButton("ColorButton",
                   {color.x, color.y, color.z, color.w},
                   0,
                   ImVec2(20, 20));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 0.5f);

        if (ImGui::IsItemClicked())
        {
            ImGui::OpenPopup("ColorPickerPopup");
        }

        if (ImGui::BeginPopup("ColorPickerPopup"))
        {
            pickerWasOpen = true;

            ImGui::ColorPicker4("##picker", reinterpret_cast<float*>(&color));

            ImGui::EndPopup();
        }
        else if (pickerWasOpen)
        {
            pickerWasOpen = false;
        }
        ImGui::PopID();
    }

    void LevelCreationUISystem::onCreateUI(const ui::CreateLevel& event)
    {
        is_active = true;
    }

    void LevelCreationUISystem::DrawLevelForm(const ImGuiViewport* viewport, ImFont* font)
    {
        ImGui::PushFont(font);
        ImGui::SetNextWindowPos(viewport->Pos);
        const auto viewportSize = viewport->Size;
        ImGui::SetNextWindowSize(viewportSize);
        styleWindow(viewportSize);
        ImGui::Begin("Level Creation", nullptr,
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse
        );

        const auto padding = ImGui::GetStyle().ItemSpacing;
        const auto windowPos = ImGui::GetWindowPos();
        const auto windowSize = ImGui::GetWindowSize();


        ImGui::PushFont(ui::FontManager::getFont("pixeloid-bold-30"));
        const ImVec2 textSize = ImGui::CalcTextSize("Level Creation");
        ImGui::SetCursorPos({(windowSize.x - textSize.x) * 0.5f, windowPos.y + padding.y});
        ImGui::Text("Level Creation");
        ImGui::PopFont();

        ImGui::Text("Level Name:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.2f);
        ImGui::InputText("##Level Name", name_input_buffer, IM_ARRAYSIZE(name_input_buffer));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {10, 10});
        ImGui::Text("Level Audio:");
        ImGui::SameLine();
        const std::vector<std::string> soundTracks = { "Option 1", "Option 2", "Option 3" }; //TODO in constructor
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.2f);
        DropDown("##SoundTracks", current_audio_item, soundTracks);

        ImGui::Text("Level Background:");
        ImGui::SameLine();
        const std::vector<std::string> items = { "Option A", "Option B", "Option C" }; //TODO in constructor
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.2f);
        DropDown("##Backgrounds", current_bg_item, items);
        ImGui::PopStyleVar();


        if(ImGui::Checkbox("##BGColor", &use_solid_bg_color))
        {
            if(use_solid_bg_color) use_gradient_bg_color = false;
        }
        if (use_solid_bg_color)
        {
            ColorPicker(picker_was_open, clear_color, "ClearColPicker");
        }
        ImGui::SameLine();
        ImGui::Text("Solid Background Color");

        if(ImGui::Checkbox("##GradientBGColor", &use_gradient_bg_color))
        {
            if(use_gradient_bg_color) use_solid_bg_color = false;
        }
        ImGui::SameLine();
        ImGui::Text("Gradient Background Color");

        if (use_gradient_bg_color)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
            auto posX = ImGui::GetCursorPosX();

            ImGui::Bullet();
            ImGui::Text("Gradient Top Color:");
            ColorPicker(picker_was_open, top_gradient, "TopGradientPicker");

            ImGui::SetCursorPosX(posX);
            ImGui::Bullet();
            ImGui::Text("Gradient Bottom Color:");
            ColorPicker(picker_was_open, bottom_gradient, "BottomGradientPicker");
        }

        if (ImGui::Button("Cancel", ui::CalculateButtonSize(padding, "Cancel")))
        {
            //TODO level + meta + objects wieder löschen
            is_active = false;
        }


        popWindowStyle();
        ImGui::PopFont();
        ImGui::End();
    }

    void LevelCreationUISystem::createUI()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        DrawLevelForm(viewport, ui::FontManager::getFont("PixeloidSans"));
    }


    void LevelCreationUISystem::update(const float deltaTime)
    {
        createUI();
    }
}
