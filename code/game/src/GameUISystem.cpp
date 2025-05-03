#include "GameUISystem.h"

#include <iostream>
#include <engine/rendering/MVPMatrixHelper.h>

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
                // Deselect if clicking same cell
                selected_screen_pixel = nullptr;
                std::cout << "Deselected grid cell." << std::endl;
            }
            else
            {
                // Select new cell
                selected_screen_pixel = std::make_unique<ImVec2>(snappedX, snappedY);
                std::cout << "Selected grid cell: " << snappedX << ", " << snappedY << std::endl;
            }
        }

        // when drawing:
        if (selected_screen_pixel)
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
        int tileIndex = 0;

        for (int i = 0; i < uvs.size(); ++i)
        {
            const auto& uv = uvs[i];

            std::string buttonId = name + "_Tile_" + std::to_string(i);
            ImVec2 uv0(uv.x, -uv.y);
            ImVec2 uv1(uv.z, -uv.w);

            if (ImGui::ImageButton(buttonId.c_str(), texture.getID(),
                                   ImVec2(tileSize, tileSize), uv0, uv1))
            {
                engine::ecs::EventDispatcher::dispatcher.trigger(TileSelectedEvent{
                    &texture, uv, *selected_screen_pixel
                });
                //TODO Button highlighten
            }

            if (++tileIndex % tilesPerRow != 0)
                ImGui::SameLine();
        }
        ImGui::Separator();
    }

    void GameUISystem::visualizeSingleTextureUI(const engine::rendering::Texture& texture, const std::string& name,
                                                const float tileSize) const
    {
        std::string btnID = name + "_full";
        if (ImGui::ImageButton(btnID.c_str(), texture.getID(),
                               ImVec2(tileSize, tileSize), ImVec2(0, 0), ImVec2(1, -1)))
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(
                TileSelectedEvent{&texture, {0, 0, 1, 1}, *selected_screen_pixel});
        }
    }

    void GameUISystem::DrawTileSelectionPanel()
    {
        int width, height;
        glfwGetWindowSize(game.getWindow(), &width, &height);
        ImGui::SetNextWindowPos(ImVec2(width * 0.75f, 0.f));
        ImGui::SetNextWindowSize(ImVec2(width * 0.25f, height));
        ImGui::Begin("Tile Panel");

        ImGui::Separator();
        ImGui::Text("1.) Click to select a grid position\n2.) Select a tile or texture:");

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        float totalSpacing = itemSpacing * (tilesPerRow + 2);
        float tileSize = (availableWidth - totalSpacing) / tilesPerRow;

        ImGui::Text("Textures:");
        int tileIndex = 0;
        for (const auto& [name, texture] : engine::rendering::TextureManager::getAllTextures())
        {
            visualizeSingleTextureUI(texture, name, tileSize);
            if (++tileIndex % tilesPerRow != 0)
                ImGui::SameLine();
        }
        ImGui::Separator();

        for (const auto& [name, texture] : engine::rendering::TextureManager::getAllTileSets())
        {
            visualizeTileSetUI(texture, name, tileSize);
        }

        ImGui::End();
    }

    void GameUISystem::createCustomUI()
    {
        DrawTileSelectionPanel();
        DrawGrid( 1.f * pixelsPerMeter);
    }

    void GameUISystem::updateUI()
    {

        createCustomUI();
    }
}
