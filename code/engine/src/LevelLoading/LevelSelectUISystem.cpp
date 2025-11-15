#include "engine/levelLoading/LevelSelectUISystem.h"
#include <string>
#include <vector>
#include "engine/rendering/TextureManager.h"
#include <engine/userInterface/FontManager.h>
#include "engine/userInterface/UIConstants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/levelloading/LevelManager.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::engine::levelLoading
{
    void LevelSelectUISystem::DrawLevelButtons()
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().ItemSpacing.x * 2.f);
        ImGui::BeginChild("Lvl Select Buttons");

        auto metaData = LevelManager::getMetaData();
        std::ranges::sort(metaData, [](const LevelMeta& a, const LevelMeta& b)
        {
            return a.id < b.id;
        });

        constexpr int columns = 3;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        const float contentWidth = ImGui::GetContentRegionAvail().x;
        const float buttonWidth = (contentWidth - 2 * spacing * (columns)) / columns;

        float buttonsOnRow = 0.f;
        for (int i = 0; i < metaData.size(); ++i)
        {
            if (ImGui::ImageButton(metaData[i].name.c_str(),
                                   rendering::TextureManager::getUITexture("LevelButton1")->getID(),
                                   ImVec2(buttonWidth, buttonWidth)))
            {
                ecs::EventDispatcher::dispatcher.trigger(ecs::GameStateChange(GameState::Level, i));
            }

            const ImVec2 buttonMin = ImGui::GetItemRectMin();
            const ImVec2 buttonSize = ImGui::GetItemRectSize();

            const auto buttonCenter = ImVec2(
                buttonMin.x + buttonSize.x * 0.5f,
                buttonMin.y + buttonSize.y * 0.5f
            );

            if (!metaData[i].previewImageName.empty())
            {
                const ImVec2 overlaySize(buttonWidth * 0.6f, buttonWidth * 0.6f);

                const auto overlayMin = ImVec2(
                    buttonCenter.x - overlaySize.x * 0.5f,
                    buttonCenter.y - overlaySize.y * 0.5f
                );
                const auto overlayMax = ImVec2(
                    overlayMin.x + overlaySize.x,
                    overlayMin.y + overlaySize.y
                );
                const auto overlayTex = rendering::TextureManager::getUITexture(metaData[i].previewImageName)->
                    getID();
                ImGui::GetWindowDrawList()->AddImage(overlayTex, overlayMin, overlayMax, {0.f, 1.f}, {1.f, 0.f});
            }
            const auto textSize = ImGui::CalcTextSize(metaData[i].name.c_str());
            const auto textPos = ImVec2(
                buttonMin.x + (buttonSize.x - textSize.x) * 0.5f,
                buttonMin.y + (buttonSize.y - textSize.y) * 0.95f
            );

            ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255), metaData[i].name.c_str());

            buttonsOnRow++;
            if (buttonsOnRow < columns && i < metaData.size() - 1)
                ImGui::SameLine();
            else
                buttonsOnRow = 0;
        }
        ImGui::EndChild();
    }

    void LevelSelectUISystem::DrawLevelSelect(const ImGuiViewport* viewport, ImFont* font)
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
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse
        );

        const auto windowSize = ImGui::GetWindowSize();
        const auto windowPos = ImGui::GetWindowPos();

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::getUITexture("LvlSelectBG1")->getID(),
            windowPos,
            ImVec2(windowPos.x + windowSize.x,
                   windowPos.y + windowSize.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        if(borderColorChanged)
        {
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::Red);
        }
        DrawLevelButtons();
        if(borderColorChanged)
        {
            ImGui::PopStyleColor();
        }

        ImGui::GetWindowDrawList()->AddImage(
            rendering::TextureManager::getUITexture("LvlSelectBGTop1")->getID(),
            windowPos,
            ImVec2(windowPos.x + windowSize.x,
                   windowPos.y + windowSize.y),
            {0.f, 1.f},
            {1.f, 0.f}
        );

        const auto padding = ImGui::GetStyle().ItemSpacing;

        ImGui::PushFont(ui::FontManager::getFont("pixeloid-bold-26"));

        const auto editTextSize = ImGui::CalcTextSize("Edit Mode");
        const ImVec2 editButtonSize = {editTextSize.x + padding.x, editTextSize.y + padding.y};
        ImGui::SetCursorPos({windowPos.x + padding.x, windowPos.y + padding.y});

        bool pushedStyle = false;

        if (editModeActive)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::Cyan);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::pastelNeonViolet);
            pushedStyle = true;
        }
        if (ImGui::Button("Edit Mode", editButtonSize))
        {
            editModeActive = !editModeActive;
            ecs::EventDispatcher::dispatcher.trigger(ui::EditModeButtonPress{editModeActive});
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {20.f, 20.f});
            ImGui::BeginTooltip();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Experimental!");
            ImGui::EndTooltip();
            ImGui::PopStyleVar();
        }
        if (pushedStyle)
        {
            ImGui::PopStyleColor(3);
        }

        if(editModeActive)
        {
            ImGui::SameLine();
            const auto addLvlTextSize = ImGui::CalcTextSize("Add Level");
            const ImVec2 addButtonSize = {addLvlTextSize.x + padding.x, addLvlTextSize.y + padding.y};
            if (ImGui::Button("Add Level", addButtonSize))
            {
                ecs::EventDispatcher::dispatcher.trigger(ui::CreateLevel{});
            }

            ImGui::SameLine();
            if (isDeletingLvl)
            {
                ImGui::PushStyleColor(ImGuiCol_Border, UINeonColors::Red);
            }
            borderColorChanged = isDeletingLvl;

            const auto deleteLvlTextSize = ImGui::CalcTextSize("Delete Level");
            const ImVec2 deleteButtonSize = {deleteLvlTextSize.x + padding.x, deleteLvlTextSize.y + padding.y};
            if (ImGui::Button("Delete Level", deleteButtonSize))
            {
                isDeletingLvl = !isDeletingLvl;
            }

            if (borderColorChanged)
            {
                ImGui::PopStyleColor();
            }

        }
        ImGui::PopFont();


        ImGui::PushFont(ui::FontManager::getFont("pixeloid-bold-30"));

        const ImVec2 textSize = ImGui::CalcTextSize("Select a Level");
        ImGui::SetCursorPos({(windowSize.x - textSize.x) * 0.5f, windowPos.y + padding.y});
        ImGui::Text("Select a Level");

        const auto buttonTextSize = ImGui::CalcTextSize("Exit");
        const ImVec2 buttonSize = {buttonTextSize.x + padding.x * 2, buttonTextSize.y + padding.y};
        ImGui::SetCursorPos({(windowSize.x - buttonSize.x) * 0.5f, windowSize.y - buttonSize.y - padding.y * 0.5f});
        if (ImGui::Button("Exit", buttonSize))
        {
            ecs::EventDispatcher::dispatcher.trigger(ecs::GameExit(true));
        }
        ImGui::PopFont();

        popWindowStyle();
        ImGui::PopFont();
        ImGui::End();
    }

    void LevelSelectUISystem::createLevelSelection()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        DrawLevelSelect(viewport, ui::FontManager::getFont("PixeloidSans"));
    }


    void LevelSelectUISystem::update(const float deltaTime)
    {
        createLevelSelection();
    }
}
