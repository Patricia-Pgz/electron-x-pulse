#include "GameUISystem.h"

#include "Constants.h"
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

    void GameUISystem::DrawTileSelectionPanel() const
    {
        const engine::rendering::Texture& tileSet = engine::rendering::TextureManager::get("tileset");
        const auto tileUVs = tileSet.generateTileUVs(8,8);
        int width;
        int height;
        glfwGetWindowSize(game.getWindow(), &width, &height);
        ImGui::SetNextWindowPos(ImVec2(width * 0.75f, 0.f));
        ImGui::SetNextWindowSize(ImVec2(width * 0.25f, height));

        ImGui::Begin("Tile Panel");

        ImGui::Separator();
        ImGui::Text("1.) Select a position on the grid\n2.) Select a tile to place:");

        const int tilesPerRow = 4;
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float itemSpacing = ImGui::GetStyle().ItemSpacing.x;

        // Subtract the total spacing between tiles + a small offset
        float totalSpacing = itemSpacing * (tilesPerRow + 2);
        float tileSize = (availableWidth - totalSpacing) / tilesPerRow;

        // Tile selection grid
        int tileIndex = 0;

        for (int i = 0; i < tileUVs.size(); ++i)
        {
            const auto& uv = tileUVs[i];

            std::string buttonId = "TileButton_" + std::to_string(i);

            if (ImGui::ImageButton(buttonId.c_str(), tileSet.getID(),
                                   ImVec2(tileSize, tileSize), ImVec2(uv.x, uv.y), ImVec2(uv.z, uv.w)))
            {
                // Tile i selected!
            }
            if (++tileIndex % tilesPerRow != 0)
                ImGui::SameLine();
        }

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
