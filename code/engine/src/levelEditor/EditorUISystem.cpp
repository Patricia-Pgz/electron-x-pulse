#include "engine/levelEditor/EditorUISystem.h"
#include <iostream>

#include "../../../game/src/Game.h"
#include "engine/Constants.h"
#include "engine/userInterface/UIConstants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/rendering/Texture.h"
#include "engine/rendering/TextureManager.h"
#include "engine/userInterface/FontManager.h"

namespace gl3::engine::editor
{
    void EditorUISystem::onMouseScroll(const context::MouseScrollEvent& event)
    {
        const auto offset = static_cast<float>(event.yOffset * pixelsPerMeter);
        game_.getContext().moveCameraX(offset);
        camera_x_offset += static_cast<float>(event.yOffset);
    }

    void EditorUISystem::DrawGrid(const float gridSpacing)
    {
        const ImVec2 screenSize = imgui_io_->DisplaySize;
        grid_center = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        const int verticalLines = static_cast<int>(screenSize.x / gridSpacing);
        const int horizontalLines = static_cast<int>(screenSize.y / gridSpacing);

        // vertical lines
        for (int i = -verticalLines; i <= verticalLines; ++i)
        {
            const float xPos = (static_cast<float>(i) + grid_offset) * gridSpacing;
            drawList->AddLine(ImVec2(grid_center.x + xPos, screenSize.y), ImVec2(grid_center.x + xPos, -screenSize.y),
                              IM_COL32(100, 100, 100, 255));
        }

        // horizontal lines
        for (int j = -horizontalLines; j <= horizontalLines; ++j)
        {
            const float yPos = (static_cast<float>(j) + grid_offset) * gridSpacing;
            drawList->AddLine(ImVec2(screenSize.x, grid_center.y + yPos), ImVec2(-screenSize.x, grid_center.y + yPos),
                              IM_COL32(100, 100, 100, 255));
        }

        if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            float scrollOffsetX = ImGui::GetScrollX();
            game_.getContext().moveCameraX(scrollOffsetX * gridSpacing);

            // Snap click to grid cell
            ImVec2 relativePos(mousePos.x - grid_center.x, mousePos.y - grid_center.y);

            const int cellX = static_cast<int>(std::round(relativePos.x / gridSpacing));
            const int cellY = static_cast<int>(std::round(relativePos.y / gridSpacing));

            std::cout << "cellX: " + std::to_string(cellX) + "cellY: " + std::to_string(cellY);

            if (ImVec2 clickedCell(static_cast<float>(cellX), static_cast<float>(cellY)); selected_grid_cell &&
                selected_grid_cell->x == clickedCell.x && selected_grid_cell->y == clickedCell.y)
            {
                selected_grid_cell.reset();
            }
            else
            {
                selected_grid_cell = std::make_unique<ImVec2>(clickedCell);
            }
        }

        if (selected_grid_cell)
        {
            const int cellX = static_cast<int>(selected_grid_cell->x);
            const int cellY = static_cast<int>(selected_grid_cell->y);
            const auto cellCenter = ImVec2(grid_center.x + static_cast<float>(cellX) + camera_x_offset * gridSpacing,
                                           grid_center.y + static_cast<float>(cellY) * gridSpacing);
            const auto topLeft = ImVec2(cellCenter.x - gridSpacing * 0.5f, cellCenter.y - gridSpacing * 0.5f);
            const auto cellBottomRight = ImVec2(cellCenter.x + gridSpacing * 0.5f, cellCenter.y + gridSpacing * 0.5f);

            drawList->AddRect(topLeft, cellBottomRight, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
        }
    }

    void EditorUISystem::visualizeTileSetUI(const rendering::Texture& texture, const std::string& name,
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
            ImVec2 uv0(uv.x, uv.w);
            ImVec2 uv1(uv.z, uv.y);

            ImGui::GetStyle().FrameRounding = 1.0;
            ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);

            if (ImGui::ImageButton(buttonId.c_str(), texture.getID(),
                                   ImVec2(tileSize, tileSize), uv0, uv1) && selected_grid_cell)
            {
                ecs::EventDispatcher::dispatcher.trigger(TileSelectedEvent{
                    {
                        {
                            selected_grid_cell->x, -selected_grid_cell->y, 0.f
                        },
                        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                        selected_tag, is_triangle,
                        name, {selected_scale.x, selected_scale.y, 0.f}, uv
                    }
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

    void EditorUISystem::visualizeSingleTextureUI(const rendering::Texture& texture, const std::string& name,
                                                  const float tileSize) const
    {
        std::string btnID = name + "_full";
        ImGui::GetStyle().FrameRounding = 1.0;
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet); // normal
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2); // hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
        if (ImGui::ImageButton(btnID.c_str(), texture.getID(),
                               ImVec2(tileSize, tileSize), ImVec2(0, 0), ImVec2(1, -1)) && selected_grid_cell)
        {
            ecs::EventDispatcher::dispatcher.trigger(
                TileSelectedEvent{
                    {
                        {
                            selected_grid_cell->x, -selected_grid_cell->y, 0.f
                        },
                        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                        selected_tag, is_triangle,
                        name, {selected_scale.x, selected_scale.y, 0.f}, {0, 0, 1, 1}
                    }
                });
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(btnID.c_str());
        }
        ImGui::PopStyleColor(3);
    }

    void EditorUISystem::highlightSelectedButton(const std::vector<std::string>& buttonIDs)
    {
        int counter = 0;
        for (const auto& id : buttonIDs)
        {
            const bool isSelected = (selected_tag == id);
            ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);

            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet2);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
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
                    selected_tag = "undefined";
                else
                {
                    selected_tag = id;
                    tag_input_buffer[0] = '\0';
                }
            }

            if (counter != buttonIDs.size() - 1)
            {
                ImGui::SameLine();
            }
            counter++;
        }
    }

    void styleWindow()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowPadding = ImVec2(10, 10);
        style.WindowRounding = 3.0f;
        ImGui::GetStyle().FrameRounding = 5.0;

        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, UINeonColors::pastelNeonViolet);

        style.ItemSpacing = ImVec2(10, 10);
    }

    void EditorUISystem::DrawTileSelectionPanel()
    {
        const ImVec2 screenSize = imgui_io_->DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screenSize.x * 0.7f, 0.f));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.3f, screenSize.y));
        styleWindow();
        ImGui::PushStyleColor(ImGuiCol_Text, UINeonColors::windowBgColor);
        ImGui::PushFont(ui::FontManager::getFont("PixeloidSans-Bold"));
        ImGui::Begin("Tile Panel", nullptr, flags_);
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::PushFont(ui::FontManager::getFont("PixeloidSans"));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("1.) Click on grid to select position");

        ImGui::Text("2.) Select shape:");
        ImGui::PushStyleColor(ImGuiCol_CheckMark, UINeonColors::pastelNeonViolet2);
        if (ImGui::RadioButton("Rectangle", !is_triangle))
            is_triangle = false;
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_CheckMark, UINeonColors::pastelNeonViolet2);
        if (ImGui::RadioButton("Triangle", is_triangle))
            is_triangle = true;
        ImGui::PopStyleColor(2);

        ImGui::Text("3.) Scale:");
        auto itemWidth = ImGui::GetContentRegionAvail().x * 0.4f;
        ImGui::SetNextItemWidth(itemWidth);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UINeonColors::pastelNeonViolet2);
        if (ImGui::InputFloat("X", &selected_scale.x, 0.1f, 1.0f, "%.2f"))
        {
            if (selected_scale.x < 0.0f)
                selected_scale.x = 0.1f;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UINeonColors::pastelNeonViolet2);
        if (ImGui::InputFloat("Y", &selected_scale.y, 0.1f, 10.f, "%.2f"))
        {
            if (selected_scale.x < 0.0f)
                selected_scale.x = 0.1f;
        }
        ImGui::PopStyleColor(4);

        ImGui::Text("4.) Select tag:");
        std::vector<std::string> tagButtonIDs{"platform", "obstacle"};
        highlightSelectedButton(tagButtonIDs);
        ImGui::Text("Custom tag:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg,
                              tag_input_buffer[0] == '\0'
                                  ? UINeonColors::pastelNeonViolet
                                  : UINeonColors::pastelNeonViolet2);
        ImGui::InputText("##tag_input", tag_input_buffer, IM_ARRAYSIZE(tag_input_buffer));
        ImGui::PopStyleColor();
        if (tag_input_buffer != selected_tag && tag_input_buffer[0] != '\0')
        {
            selected_tag = tag_input_buffer;
        }
        ImGui::Text("5.) Select Tile to place:");
        const float availableWidth = ImGui::GetContentRegionAvail().x;
        const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float totalSpacing = itemSpacing * (tilesPerRow + 2);
        const float tileSize = (availableWidth - totalSpacing) / tilesPerRow;

        ImGui::Separator();
        ImGui::Text("Select Visual:");

        if (ImGui::RadioButton("##Color", use_color_))
        {
            use_color_ = true;
        }
        ImGui::SameLine();

        if (use_color_)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
            ImGui::ColorButton("##ColorPreview",
                               {selected_color_.x, selected_color_.y, selected_color_.z, selected_color_.w},
                               0,
                               ImVec2(20, 20));
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y * 0.5f);

            if (ImGui::IsItemClicked())
            {
                ImGui::OpenPopup("ColorPickerPopup");
            }
        }

        ImGui::SameLine();
        ImGui::Text("Color");
        ImGui::SameLine();

        if (ImGui::RadioButton("Texture", !use_color_))
        {
            use_color_ = false;
        }

        if (use_color_)
        {
            static bool pickerWasOpen = false;

            if (ImGui::BeginPopup("ColorPickerPopup"))
            {
                pickerWasOpen = true;

                ImGui::ColorPicker4("##picker", reinterpret_cast<float*>(&selected_color_));

                ImGui::EndPopup();
            }
            else if (pickerWasOpen)
            {
                pickerWasOpen = false;

                ecs::EventDispatcher::dispatcher.trigger(
                    TileSelectedEvent{
                        {
                            {
                                selected_grid_cell->x, -selected_grid_cell->y, 0.f
                            },
                            selected_color_,
                            selected_tag, is_triangle,
                            "", {selected_scale.x, selected_scale.y, 0.f}, {0, 0, 1, 1}
                        }
                    });
            }
        }
        else
        {
            ImGui::Text("Textures:");
            int tileIndex = 0;
            for (const auto& [name, texture] : rendering::TextureManager::getAllTextures())
            {
                if (tileIndex % tilesPerRow != 0)
                    ImGui::SameLine();
                visualizeSingleTextureUI(*texture, name, tileSize);
                tileIndex++;
            }
            ImGui::Separator();

            for (const auto& [name, texture] : rendering::TextureManager::getAllTileSets())
            {
                visualizeTileSetUI(*texture, name, tileSize);
            }
        }

        ImGui::PopStyleColor(4);
        ImGui::PopFont();
        ImGui::End();
    }

    void EditorUISystem::createCustomUI()
    {
        DrawTileSelectionPanel();
        DrawGrid(1.f * pixelsPerMeter);
    }

    void EditorUISystem::update()
    {
        if (!is_active) return;
        createCustomUI();
    }
}
