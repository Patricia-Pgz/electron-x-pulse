#include "engine/levelLoading/LevelUISystem.h"
#include "engine/Constants.h"

namespace gl3::engine::levelLoading
{
    void styleWindow()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowBorderSize = 0.f;
        style.WindowPadding = ImVec2(10, 10);
        style.WindowRounding = 3.0f;
        ImGui::GetStyle().FrameRounding = 5.0;

        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, UINeonColors::pastelNeonViolet);
        ImGui::PopStyleColor(3);

        style.ItemSpacing = ImVec2(10, 10);
    }

    void CenteredText(const char* text, ImVec2 windowSize)
    {
        ImVec2 textSize = ImGui::CalcTextSize(text);
        float offsetX = (windowSize.x - textSize.x) * 0.5f;

        ImGui::SetCursorPosX(offsetX);
        ImGui::Text("%s", text);
    }

    void LevelUISystem::createLevelSelection()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Set next window position and size to match the viewport
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        // Flags to remove window decorations and background
        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;

        styleWindow();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, UINeonColors::softPastelPink);
        ImGui::Begin("Level Select", nullptr, flags);
        ImGui::PopStyleColor();

        std::vector<std::string> levelNames = {
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
        ImVec2 screenCenter = {viewport->Size.x * 0.5f, viewport->Size.y * 0.5f};
        ImGui::SetCursorPosY(viewport->Size.y * 0.05f);
        ImGui::PushFont(loadedFonts["PixeloidSans-Bold.ttf"]); //TODO UISystem load this font bigger
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(40, 40));
        CenteredText("Select a Level", viewport->Size);
        ImGui::PopStyleVar();
        ImGui::PopFont();
        ImGui::Separator(); //TODO hier lieber image?

        float fullWidth = ImGui::GetContentRegionAvail().x;

        int columns = 4; // or calculate based on width
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float contentWidth = ImGui::GetContentRegionAvail().x;
        float buttonWidth = (contentWidth - spacing * (columns - 1)) / columns;
        float buttonHeight = 60.0f; // fixed height

        int buttonsOnRow = 0;
        ImGui::PushFont(loadedFonts["PixeloidSans.ttf"]);
        for (size_t i = 0; i < levelNames.size(); ++i)
        {
            if (ImGui::Button(levelNames[i].c_str(), ImVec2(buttonWidth, buttonWidth)))
            {
                //onLevelSelected(levelNames[i]);
            }

            buttonsOnRow++;
            if (buttonsOnRow < columns && i < levelNames.size() - 1)
                ImGui::SameLine();
            else
                buttonsOnRow = 0;
        }

        ImGui::Dummy(ImVec2(0, 20));
        ImGui::Separator();

        if (ImGui::Button("Back", ImVec2(200, 40)))
        {
            //onLevelSelected(""); // Signal to exit
        }

        ImGui::PopFont();
        ImGui::End();
    }

    void LevelUISystem::updateUI()
    {
        createLevelSelection();
    }
}
