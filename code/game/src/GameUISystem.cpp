#include "GameUISystem.h"

#include <iostream>
#include <engine/rendering/MVPMatrixHelper.h>

#include "Assets.h"
#include "Constants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/rendering/Texture.h"
#include "engine/rendering/TextureManager.h"

namespace gl3
{
    void GameUISystem::DrawGrid(const float gridSpacing)
    {
        const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        grid_center_ = ImVec2(screenSize.x * 0.5f + grid_offset, screenSize.y * 0.5f + grid_offset);
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        int verticalLines = static_cast<int>(screenSize.x / gridSpacing);
        int horizontalLines = static_cast<int>(screenSize.y / gridSpacing);

        // vertical lines
        for (int i = -verticalLines; i <= verticalLines; ++i)
        {
            float xPos = (i) * gridSpacing;
            drawList->AddLine(ImVec2(grid_center_.x + xPos, screenSize.y), ImVec2(grid_center_.x + xPos, -screenSize.y),
                              IM_COL32(100, 100, 100, 255));
        }

        // horizontal lines
        for (int j = -horizontalLines; j <= horizontalLines; ++j)
        {
            float yPos = (j) * gridSpacing;
            drawList->AddLine(ImVec2(screenSize.x, grid_center_.y + yPos), ImVec2(-screenSize.x, grid_center_.y + yPos),
                              IM_COL32(100, 100, 100, 255));
        }

        if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
        {
            ImVec2 mousePos = ImGui::GetMousePos();

            // Snap click to grid cell
            ImVec2 relativePos(mousePos.x - grid_center_.x, mousePos.y - grid_center_.y);
            float snappedX = grid_center_.x + std::floor(relativePos.x / gridSpacing) * gridSpacing;
            float snappedY = grid_center_.y + std::floor(relativePos.y / gridSpacing) * gridSpacing;

            if (selected_screen_pixel &&
                selected_screen_pixel->x == snappedX &&
                selected_screen_pixel->y == snappedY)
            {
                selected_screen_pixel = nullptr;
            }
            else
            {
                selected_screen_pixel = std::make_unique<ImVec2>(snappedX, snappedY);
            }
        }

        if (selected_screen_pixel) //TODO bei window resize falsch
        {
            ImVec2 cellMin = *selected_screen_pixel;
            ImVec2 cellMax = ImVec2(selected_screen_pixel->x + gridSpacing,
                                    selected_screen_pixel->y + gridSpacing);
            drawList->AddRect(cellMin, cellMax, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
        }
    }

    void GameUISystem::visualizeTileSetUI(const engine::rendering::Texture& texture, const std::string& name,
                                          const float tileSize) const
    {
        ImGui::Text(name.c_str());
        const auto& uvs = texture.getTileUVs();

        for (int i = 0; i < uvs.size(); ++i)
        {
            if (i % tilesPerRow != 0)
                ImGui::SameLine();
            const auto& uv = uvs[i];

            std::string buttonId = name + "_Tile_" + std::to_string(i);
            ImVec2 uv0(uv.x, -uv.y);
            ImVec2 uv1(uv.z, -uv.w);

            ImGui::GetStyle().FrameRounding = 1.0;
            ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,UINeonColors::Cyan);

            if (ImGui::ImageButton(buttonId.c_str(), texture.getID(),
                                   ImVec2(tileSize, tileSize), uv0, uv1) && selected_screen_pixel)
            {
                engine::ecs::EventDispatcher::dispatcher.trigger(TileSelectedEvent{
                    &texture, uv, *selected_screen_pixel, selected_tag_
                });
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(buttonId.c_str());
            }

            ImGui::PopStyleColor(3);
        }
        ImGui::Separator();
    }

    void GameUISystem::visualizeSingleTextureUI(const engine::rendering::Texture& texture, const std::string& name,
                                                const float tileSize) const
    {
        std::string btnID = name + "_full";
        ImGui::GetStyle().FrameRounding = 1.0;
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);           // normal
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);    // hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,UINeonColors::Cyan);
        if (ImGui::ImageButton(btnID.c_str(), texture.getID(),
                               ImVec2(tileSize, tileSize), ImVec2(0, 0), ImVec2(1, -1)) && selected_screen_pixel)
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(
                TileSelectedEvent{&texture, {0, 0, 1, 1}, *selected_screen_pixel, selected_tag_});
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(btnID.c_str());
        }
        ImGui::PopStyleColor(3);
    }

    void GameUISystem::highlightSelectedButton(const std::vector<std::string>& buttonIDs)
    {
        int counter = 0;
        for (const auto& id : buttonIDs)
        {
            const bool isSelected = (selected_tag_ == id);
            ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,UINeonColors::Cyan);
            ImGui::GetStyle().FrameRounding = 1.0;

            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet2);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,UINeonColors::Cyan);
            }

            const bool pressed = ImGui::Button(id.c_str());

            if (isSelected)
            {
                ImGui::PopStyleColor(3);
            }

            ImGui::PopStyleColor(3);

            if (pressed)
            {
                if (isSelected)
                    selected_tag_ = "undefined";
                else
                    selected_tag_ = id;
            }

            if (counter != buttonIDs.size() - 1)
            {
                ImGui::SameLine();
            }
            counter++;
        }
    }

    void styleWindow() {
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowBorderSize = 2.f;
        style.WindowPadding = ImVec2(10, 10);
        style.WindowRounding = 1.0f;

        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, UINeonColors::pastelNeonViolet);

        style.ItemSpacing = ImVec2(10,10);
    }

    void GameUISystem::DrawTileSelectionPanel()
    {
        const ImVec2 screenSize = imgui_io->DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screenSize.x * 0.7f, 0.f));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.3f, screenSize.y));
        styleWindow();
        ImGui::PushStyleColor(ImGuiCol_Text, UINeonColors::windowBgColor);
        ImGui::PushFont(loadedFonts["PixeloidSans-Bold.ttf"]);
        ImGui::Begin("Tile Panel");
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::PushFont(loadedFonts["PixeloidSans.ttf"]);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("1.) Click on grid to select position");
        ImGui::Text("2.) Select tag:");
        std::vector<std::string> tagButtonIDs{"platform", "obstacle"};
        highlightSelectedButton(tagButtonIDs);
        char tagInputBuffer[128] = "";
        ImGui::Text("Custom tag:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet); //TODO change zu violet2 wenn selected?
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  UINeonColors::pastelNeonViolet2);
        ImGui::InputText("##tag_input", tagInputBuffer, IM_ARRAYSIZE(tagInputBuffer));
        ImGui::PopStyleColor(3);
        if (tagInputBuffer != selected_tag_ && tagInputBuffer[0] != '\0') {
            selected_tag_ = tagInputBuffer;
        }
        ImGui::Text("3.) Select Tile to place:");
        const float availableWidth = ImGui::GetContentRegionAvail().x;
        const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float totalSpacing = itemSpacing * (tilesPerRow + 2);
        const float tileSize = (availableWidth - totalSpacing) / tilesPerRow;

        ImGui::Separator();
        ImGui::Text("Textures:");
        int tileIndex = 0;
        for (const auto& [name, texture] : engine::rendering::TextureManager::getAllTextures())
        {
            if (tileIndex % tilesPerRow != 0)
                ImGui::SameLine();
            visualizeSingleTextureUI(texture, name, tileSize);
            tileIndex++;
        }
        ImGui::Separator();

        for (const auto& [name, texture] : engine::rendering::TextureManager::getAllTileSets())
        {
            visualizeTileSetUI(texture, name, tileSize);
        }

        ImGui::PopStyleColor(4);
        ImGui::PopFont();
        ImGui::End();
    }

    void GameUISystem::createCustomUI()
    {

        DrawTileSelectionPanel();
        DrawGrid(1.f * pixelsPerMeter);
    }

    void GameUISystem::updateUI()
    {
        createCustomUI();
    }
}
