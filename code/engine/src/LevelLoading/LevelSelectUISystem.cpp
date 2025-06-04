#include "engine/levelLoading/LevelSelectUISystem.h"
#include <string>
#include <vector>
#include "engine/rendering/TextureManager.h"
#include <engine/userInterface/FontManager.h>
#include "engine/Constants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"

namespace gl3::engine::levelLoading
{
    void styleWindow(const ImVec2 windowSize)
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, UINeonColors::Cyan);
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = 5.0;
        style.WindowBorderSize = 0.f;
        style.WindowPadding = {windowSize.x * 0.075f, windowSize.y * 0.145f};
        style.ItemSpacing = ImVec2(20, 20);
    }

    void popStyle()
    {
        ImGui::PopStyleColor(8);
    }

    void DrawLevelButtons()
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x);
        ImGui::BeginChild("Lvl Select Buttons");

        const std::vector<std::string> levelNames = {
            //TODO laden von level files
            "Tutorial",
            "Forest Frenzy",
            "Desert Dash",
            "Cave Chaos",
            "Icebound Isles",
            "Sky Fortress",
            "Lava Lake",
            "Dark Dungeon",
            "Boss Battle",
            "Final Showdown"
        };

        constexpr float columns = 4.f;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float contentWidth = ImGui::GetContentRegionAvail().x;
        const float buttonWidth = (contentWidth - spacing * (columns)) / columns;

        float buttonsOnRow = 0.f;
        for (size_t i = 0; i < levelNames.size(); ++i)
        {
            if (ImGui::ImageButton(levelNames[i].c_str(),
                                   rendering::TextureManager::getUITexture("LevelButton1").getID(),
                                   ImVec2(buttonWidth, buttonWidth))) //TODO (www.freepik.com)
            {
                //onLevelSelected(levelNames[i]);
            }

            ImVec2 buttonMin = ImGui::GetItemRectMin();
            ImVec2 buttonSize = ImGui::GetItemRectSize();

            ImVec2 buttonCenter = ImVec2(
                buttonMin.x + buttonSize.x * 0.5f,
                buttonMin.y + buttonSize.y * 0.5f
            );

            ImVec2 overlaySize(buttonWidth * 0.7f, buttonWidth * 0.7f);

            ImVec2 overlayMin = ImVec2(
                buttonCenter.x - overlaySize.x * 0.5f,
                buttonCenter.y - overlaySize.y * 0.5f
            );
            ImVec2 overlayMax = ImVec2(
                overlayMin.x + overlaySize.x,
                overlayMin.y + overlaySize.y
            );

            // Draw the overlay image (centered)
            ImTextureID overlayTex = rendering::TextureManager::get("geometry-dash").getID();
            ImGui::GetWindowDrawList()->AddImage(overlayTex, overlayMin, overlayMax, {0.f, 1.f}, {1.f, 0.f});

            ImVec2 textSize = ImGui::CalcTextSize(levelNames[i].c_str());
            // Compute centered text position
            ImVec2 textPos = ImVec2(
                buttonMin.x + (buttonSize.x - textSize.x) * 0.5f,
                buttonMin.y + (buttonSize.y - textSize.y) * 0.95f
            );

            ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255), levelNames[i].c_str());

            buttonsOnRow++;
            if (buttonsOnRow < columns && i < levelNames.size() - 1)
                ImGui::SameLine();
            else
                buttonsOnRow = 0;
        }
        ImGui::EndChild();
    }

    void DrawLevelSelect(const ImGuiViewport* viewport, ImFont* font)
    {
        ImGui::PushFont(font);
        ImGui::SetNextWindowPos(viewport->Pos);
        const auto viewportSize = viewport->Size;
        ImGui::SetNextWindowSize(viewportSize);
        styleWindow(viewportSize);
        ImGui::Begin("FixedHeaderOverlay", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize|
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
        );

        const auto windowSize = ImGui::GetWindowSize();
        const auto windowPos = ImGui::GetWindowPos();

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::getUITexture("LvlSelectBG1").getID(),
            windowPos,
            ImVec2(windowPos.x + windowSize.x,
                   windowPos.y + windowSize.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        DrawLevelButtons();

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::getUITexture("LvlSelectBGTop1").getID(),
            windowPos,
            ImVec2(windowPos.x + windowSize.x,
                   windowPos.y + windowSize.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        const auto padding = ImGui::GetStyle().ItemSpacing;

        ImGui::PushFont(ui::FontManager::getFont("pixeloid-bold-26"));
        const ImVec2 textSize = ImGui::CalcTextSize("Select a Level");
        ImGui::SetCursorPos({(windowSize.x - textSize.x) * 0.5f, windowPos.y + padding.y});
        ImGui::Text("Select a Level");

        const auto buttonTextSize = ImGui::CalcTextSize("Exit");
        const ImVec2 buttonSize = {buttonTextSize.x + padding.x * 2, buttonTextSize.y + padding.y * 2};
        ImGui::SetCursorPos({(windowSize.x - buttonSize.x) * 0.5f, windowSize.y - buttonSize.y - padding.y * 0.5f});
        if (ImGui::Button("Exit", buttonSize))
        {
            ecs::EventDispatcher::dispatcher.trigger(ecs::GameExit(true));
        }
        ImGui::PopFont();

        popStyle();
        ImGui::PopFont();
        ImGui::End();
    }

    void LevelSelectUISystem::createLevelSelection()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        DrawLevelSelect(viewport, ui::FontManager::getFont("PixeloidSans"));
    }

    void LevelSelectUISystem::update()
    {
        createLevelSelection();
    }
}
