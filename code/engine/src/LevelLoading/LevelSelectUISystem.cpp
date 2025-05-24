#include "engine/levelLoading/LevelSelectUISystem.h"

#include <imgui_internal.h>

#include "engine/Constants.h"

namespace gl3::engine::levelLoading
{
    void styleWindow(const ImVec2 windowSize)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.f;
        style.WindowPadding = ImVec2(windowSize.x * 0.08, windowSize.y * 0.14);
        ImGui::GetStyle().FrameRounding = 5.0;

        style.ItemSpacing = ImVec2(20, 20);
    }

    void CenteredText(const char* text, const ImVec2 windowSize)
    {
        const ImVec2 textSize = ImGui::CalcTextSize(text);
        const float offsetX = (windowSize.x - textSize.x) * 0.5f;

        ImGui::SetCursorPosX(offsetX);
        ImGui::Text("%s", text);
    }

    void DrawFixedHeadingWindow(const ImGuiViewport* viewport, ImFont* headingFont)
    {
        // Fixed overlay window (no scroll, no interaction)
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize({viewport->Size.x, (viewport->Size.y * 0.1f)});
        ImGui::Begin("FixedHeaderOverlay", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoBackground
        );
        ImGui::SetCursorPosY(viewport->Size.y * 0.03f);
        ImGui::PushFont(headingFont); //TODO UISystem load this font bigger
        CenteredText("Select a Level", viewport->Size);
        ImGui::PopFont();
        ImGui::End();
        ImGui::BringWindowToDisplayFront(ImGui::FindWindowByName("FixedHeaderOverlay"));
    }

    void DrawLevelButtons()
    {
        std::vector<std::string> levelNames = {
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

        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);

        float buttonsOnRow = 0.f;
        for (size_t i = 0; i < levelNames.size(); ++i)
        {
            if (ImGui::ImageButton(levelNames[i].c_str(), rendering::TextureManager::get("LevelButton1").getID(),
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
            ImTextureID overlayTex = rendering::TextureManager::get("Player").getID();
            ImGui::GetWindowDrawList()->AddImage(overlayTex, overlayMin, overlayMax, {0.f, 1.f}, {1.f, 0.f});

            ImVec2 textSize = ImGui::CalcTextSize(levelNames[i].c_str());
            // Compute centered text position
            ImVec2 textPos = ImVec2(
                buttonMin.x + (buttonSize.x - textSize.x) * 0.5f,
                buttonMin.y + (buttonSize.y - textSize.y) * 0.95f
            );

            // Draw overlay text without modifying cursor
            ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255), levelNames[i].c_str());

            buttonsOnRow++;
            if (buttonsOnRow < columns && i < levelNames.size() - 1)
                ImGui::SameLine();
            else
                buttonsOnRow = 0;
        }
        ImGui::PopStyleColor(3);
    }

    void DrawLevelSelect(const ImGuiViewport* viewport, ImFont* font)
    {
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::PushFont(font);

        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        styleWindow(viewport->Size);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, UINeonColors::softPastelPink);
        ImGui::Begin("Level Select", nullptr, flags);
        ImGui::PopStyleColor();

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::get("LvlSelectBG1").getID(),
            ImGui::GetWindowPos(),
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
                   ImGui::GetWindowPos().y + ImGui::GetWindowSize().y)
        );

        DrawLevelButtons();

        /*if (ImGui::Button("Back", ImVec2(200, 40)))
        {
            //onLevelSelected(""); // Signal to exit
        }*/

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::get("LvlSelectBGTop1").getID(),
            ImGui::GetWindowPos(),
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x,
                   ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        ImGui::PopFont();
        ImGui::End();
    }

    void LevelSelectUISystem::createLevelSelection()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        DrawFixedHeadingWindow(viewport, loadedFonts["PixeloidSans-Bold.ttf"]);
        DrawLevelSelect(viewport, loadedFonts["PixeloidSans.ttf"]);
    }

    void LevelSelectUISystem::updateUI()
    {
        createLevelSelection();
    }
}
