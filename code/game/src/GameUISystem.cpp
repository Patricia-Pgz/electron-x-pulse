#include "GameUISystem.h"

#include <iostream>

#include "Constants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/rendering/Texture.h"
#include "engine/rendering/TextureManager.h"

namespace gl3
{
    void GameUISystem::DrawGrid(const float gridSpacing)
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
    }

    constexpr int tilesPerRow = 4;

    void visualizeTileSetUI(const engine::rendering::Texture& texture, const std::string& name, const float tileSize)
    {
        ImGui::Text("Texture: %s", name.c_str());
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
                engine::ecs::EventDispatcher::dispatcher.trigger(TileSelectedEvent{ &texture, uv });
            }

            if (++tileIndex % tilesPerRow != 0)
                ImGui::SameLine();
        }
        ImGui::Separator();
    }

    void visualizeSingleTextureUI(const engine::rendering::Texture& texture, const std::string& name, const float tileSize)
    {
        ImGui::Text("Texture: %s", name.c_str());

        std::string btnID = name + "_full";
        if (ImGui::ImageButton(btnID.c_str(), texture.getID(),
                               ImVec2(tileSize, tileSize), ImVec2(0, 0), ImVec2(1, -1)))
        {
            engine::ecs::EventDispatcher::dispatcher.trigger(TileSelectedEvent{ &texture, {0, 0, 1, 1} });
        }
        ImGui::Separator();
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

        for (const auto& [name, texture] : engine::rendering::TextureManager::getAllTextures())
        {
            visualizeSingleTextureUI(texture,name,tileSize);
        }

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
    createCustomUI();
}

}
