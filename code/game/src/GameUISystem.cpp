#include "GameUISystem.h"

#include <iostream>
#include <engine/rendering/MVPMatrixHelper.h>

#include "Constants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/rendering/Texture.h"
#include "engine/rendering/TextureManager.h"

namespace gl3
{
    void GameUISystem::handleGridSelection()
    {
        if (ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse)
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            glm::vec2 worldPos = engine::rendering::MVPMatrixHelper::screenToWorld(game, mousePos.x, mousePos.y);
            selectedGridCell = ImVec2(glm::floor(worldPos).x, glm::floor(worldPos).y); // snap to tile cell
            std::cout << "Grid selected: " << selectedGridCell.x << ", " << selectedGridCell.y << std::endl;
        }
    }

    void GameUISystem::DrawGrid(const float gridSpacing) const
    {
        //TODO grid spacing evtl an beats anpassen? oder einfach beats nur markieren?

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        ImVec2 center = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);

        int verticalLines = static_cast<int>(screenSize.x / gridSpacing);
        int horizontalLines = static_cast<int>(screenSize.y / gridSpacing);

        // vertical lines
        for (int i = -verticalLines; i <= verticalLines; ++i)
        {
            float xPos = i * gridSpacing;
            drawList->AddLine(ImVec2(center.x + xPos, screenSize.y), ImVec2(center.x + xPos, -screenSize.y),
                              IM_COL32(100, 100, 100, 255));
        }

        // horizontal lines
        for (int j = -horizontalLines; j <= horizontalLines; ++j)
        {
            float yPos = j * gridSpacing;
            drawList->AddLine(ImVec2(screenSize.x, center.y + yPos), ImVec2(-screenSize.x, center.y + yPos),
                              IM_COL32(100, 100, 100, 255));
        }

        glm::vec2 topLeft = engine::rendering::MVPMatrixHelper::toScreen(game, selectedGridCell.x, selectedGridCell.y);
        glm::vec2 bottomRight = engine::rendering::MVPMatrixHelper::toScreen(
            game, selectedGridCell.x + 1, selectedGridCell.y + 1);
        drawList->AddRect({topLeft.x, topLeft.y}, {
                              bottomRight.x, bottomRight.y
                          }, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
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
                engine::ecs::EventDispatcher::dispatcher.trigger(TileSelectedEvent{&texture, uv, selectedGridCell});
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
                TileSelectedEvent{&texture, {0, 0, 1, 1}, selectedGridCell});
        }
    }

    void GameUISystem::DrawTileSelectionPanel() const
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

    void GameUISystem::createCustomUI() const
    {
        DrawTileSelectionPanel();
        DrawGrid(1.f * pixelsPerMeter);
    }

    void GameUISystem::updateUI()
    {
        handleGridSelection();
        createCustomUI();
    }
}
