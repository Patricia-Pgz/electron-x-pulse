#include "engine/levelEditor/EditorUISystem.h"
#include "../../../game/src/Game.h"
#include "engine/Constants.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/userInterface/UIConstants.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/levelloading/LevelManager.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/rendering/MVPMatrixHelper.h"
#include "engine/rendering/Texture.h"
#include "engine/rendering/TextureManager.h"
#include "engine/userInterface/FontManager.h"

namespace gl3::engine::editor
{
    void EditorUISystem::onMouseScroll(const context::MouseScrollEvent& event) const
    {
        if (!is_active || !game.isPaused() || !is_mouse_in_grid) return;
        game.getContext().moveCameraX(static_cast<float>(event.yOffset * 50.0f));
    }


    /// Saves the current levels final beat position. (For visualization)
    void EditorUISystem::onLvlComputed(const ecs::LevelLengthComputed& event)
    {
        final_beat_position = event.finalBeatIndex;
    }

    ///Delete all entities from the registry, that have their TransformComponent position at the currently selected cell's position.
    void EditorUISystem::deleteAllAtSelectedPosition() const
    {
        auto& registry = game.getRegistry();
        const auto& view = registry.view<ecs::TransformComponent, ecs::TagComponent>();
        if (selected_grid_cells.empty()) return;
        for (auto& entity : view)
        {
            const auto transform = view.get<ecs::TransformComponent>(entity);
            const auto tag = view.get<ecs::TagComponent>(entity).tag;
            if (tag == "background" || tag == "ground" || tag == "sky") continue;

            for (const auto& cell : selected_grid_cells)
            {
                if (transform.position.x == cell.x && transform.position.y == cell.y)
                {
                    //handle grouped entity
                    if (registry.any_of<ecs::ParentComponent>(entity))
                    {
                        const auto parent = registry.get<ecs::ParentComponent>(entity).parentEntity;
                        auto& groupChildren = registry.get<ecs::GroupComponent>(parent).childEntities;
                        if (!groupChildren.empty())
                        {
                            //erase entity from group
                            std::erase_if(groupChildren,
                                          [&](const entt::entity& child)
                                          {
                                              return child == entity;
                                          });
                            // recalculate parent collider
                            auto newAABB = physics::PhysicsSystem::computeGroupAABB(groupChildren, registry);
                            ecs::EntityFactory::setPosition(registry, parent, newAABB.position);
                            ecs::EntityFactory::setScale(registry, parent, newAABB.scale);
                        }
                        else
                        {
                            // delete parent if no more children
                            ecs::EntityFactory::markEntityForDeletion(parent);
                        }
                    }
                    ecs::EntityFactory::markEntityForDeletion(entity);
                }
            }
        }

        // Also remove any level objects at these positions:
        for (const auto& cell : selected_grid_cells)
        {
            levelLoading::LevelManager::removeAllObjectsAtPosition({cell.x, cell.y});
        }

        //Signal to remove items from grouping cache
        if (!selected_group_cells.empty())
        {
            ecs::EventDispatcher::dispatcher.trigger(ui::EditorGroupTileDeleted{selected_grid_cells});
        }
    }


    /// Draws the Editor grid overlay for selecting cells and placing tiles.
    void EditorUISystem::drawGrid(const float gridSpacing)
    {
        const ImVec2 screenSize = imgui_io->DisplaySize;
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
            const ImVec2 mousePos = ImGui::GetMousePos();
            const glm::vec2 worldPos = rendering::MVPMatrixHelper::screenToWorld(
                game.getContext(), mousePos.x, mousePos.y);

            // Snap click to grid cell
            const int cellX = static_cast<int>(std::round(worldPos.x));
            const int cellY = static_cast<int>(std::round(worldPos.y));

            const ImVec2 clickedCell(static_cast<float>(cellX), static_cast<float>(cellY));

            if (multi_select_enabled)
            {
                const auto it = std::ranges::find_if(selected_grid_cells,
                                                     [&](const ImVec2& cell)
                                                     {
                                                         return cell.x == clickedCell.x && cell.y == clickedCell.y;
                                                     });

                if (it != selected_grid_cells.end())
                {
                    selected_grid_cells.erase(it); // Deselect if already selected
                }
                else
                {
                    selected_grid_cells.push_back(clickedCell);
                }
            }
            else
            {
                if (!selected_grid_cells.empty() &&
                    selected_grid_cells[0].x == clickedCell.x && selected_grid_cells[0].y == clickedCell.y)
                {
                    selected_grid_cells.clear();
                }
                else
                {
                    selected_grid_cells.clear();
                    selected_grid_cells.push_back(clickedCell);
                }
            }
        }

        if (is_mouse_in_grid)
        {
            // Get mouse position in screen space
            const ImVec2 mousePosScreen = ImGui::GetMousePos();

            // Convert to world coordinates
            const glm::vec2 mousePosWorld = rendering::MVPMatrixHelper::screenToWorld(
                game.getContext(), mousePosScreen.x, mousePosScreen.y);

            // Snap to grid (round world coords)
            const int cellX = static_cast<int>(std::round(mousePosWorld.x));
            const int cellY = static_cast<int>(std::round(mousePosWorld.y));

            // Convert snapped cell back to screen space for drawing
            const glm::vec2 cellScreenPos = rendering::MVPMatrixHelper::toScreen(
                game.getContext(), static_cast<float>(cellX), static_cast<float>(cellY));
            const float cellSize = gridSpacing;

            // Define cell rectangle (centered at cellScreenPos) where hovered
            const ImVec2 cellTopLeft(cellScreenPos.x - cellSize * 0.5f, cellScreenPos.y - cellSize * 0.5f);
            const ImVec2 cellBottomRight(cellScreenPos.x + cellSize * 0.5f, cellScreenPos.y + cellSize * 0.5f);

            // Draw highlight
            drawList->AddRectFilled(cellTopLeft, cellBottomRight, IM_COL32(100, 100, 255, 100));

            // Tooltip with accurate world coordinates
            ImGui::BeginTooltip();
            ImGui::Text("Cell: (%d, %d)", cellX, cellY);
            ImGui::EndTooltip();
        }

        //highlight selected grid cells
        for (const auto& cell : selected_grid_cells)
        {
            const auto screenPos = rendering::MVPMatrixHelper::toScreen(
                game.getContext(), cell.x, cell.y);

            ImVec2 topLeft(screenPos.x - gridSpacing * 0.5f, screenPos.y - gridSpacing * 0.5f);
            ImVec2 bottomRight(screenPos.x + gridSpacing * 0.5f, screenPos.y + gridSpacing * 0.5f);

            drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
        }
        //highlight group selection
        for (const auto& cell : selected_group_cells)
        {
            const auto screenPos = rendering::MVPMatrixHelper::toScreen(
                game.getContext(), cell.x, cell.y);

            ImVec2 topLeft(screenPos.x - gridSpacing * 0.5f, screenPos.y - gridSpacing * 0.5f);
            ImVec2 bottomRight(screenPos.x + gridSpacing * 0.5f, screenPos.y + gridSpacing * 0.5f);

            drawList->AddRect(topLeft, bottomRight, IM_COL32(0, 0, 255, 255), 0.0f, 0, 2.0f);
        }

        auto worldWindowBounds = game.getContext().getWorldWindowBounds();
        if (final_beat_position >= worldWindowBounds[0] && final_beat_position <= worldWindowBounds[1])
        {
            auto finalBeatPositionScreen = rendering::MVPMatrixHelper::toScreen(
                game.getContext(), final_beat_position, 0.f);
            drawList->AddLine(
                ImVec2(finalBeatPositionScreen.x, 0.f),
                ImVec2(finalBeatPositionScreen.x, screenSize.y),
                IM_COL32(255, 0, 0, 255), // red
                2.0f // thickness
            );
        }
    }

    void EditorUISystem::visualizeTileSetUI(const rendering::Texture& texture, const std::string& name,
                                            const float tileSize)
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

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);

            if (ImGui::ImageButton(buttonId.c_str(), texture.getID(),
                                   ImVec2(tileSize, tileSize), uv0, uv1) && !selected_grid_cells.empty())
            {
                for (const auto& cell : selected_grid_cells)
                {
                    GameObject object = {
                        {
                            cell.x + selected_position_offset.x, cell.y + selected_position_offset.y, 0.f
                        },
                        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                        selected_tag, is_triangle,
                        name, {selected_scale.x, selected_scale.y, 0.f}, uv, selected_z_rotation, generate_physics_comp
                    };
                    object.zLayer = selected_layer;
                    ecs::EventDispatcher::dispatcher.trigger(ui::EditorTileSelectedEvent{object, compute_group_AABB});
                    if (compute_group_AABB)
                    {
                        selected_group_cells.push_back(cell);
                    }
                }
                selected_grid_cells.clear();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(buttonId.c_str());
            }

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();
        }
        ImGui::Separator();
    }

    void EditorUISystem::visualizeSingleTextureUI(const rendering::Texture& texture, const std::string& name,
                                                  const float tileSize)
    {
        const std::string btnID = name + "_full";
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet); // normal
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2); // hovered
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
        if (ImGui::ImageButton(btnID.c_str(), texture.getID(),
                               ImVec2(tileSize, tileSize), ImVec2(0, 0), ImVec2(1, -1))
            && !selected_grid_cells.empty())
        {
            for (const auto& cell : selected_grid_cells)
            {
                GameObject object = {
                    {
                        cell.x + selected_position_offset.x, cell.y + selected_position_offset.y, 0.f
                    },
                    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                    selected_tag, is_triangle,
                    name, {selected_scale.x, selected_scale.y, 0.f},
                    {0, 0, 1, 1}, selected_z_rotation,
                    generate_physics_comp
                };
                object.zLayer = selected_layer;
                ecs::EventDispatcher::dispatcher.trigger(
                    ui::EditorTileSelectedEvent{object, compute_group_AABB});
                if (compute_group_AABB)
                {
                    selected_group_cells.push_back(cell);
                }
            }
            selected_grid_cells.clear();
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
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, UINeonColors::Cyan);
        ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::pastelNeonViolet);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::Cyan);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    }

    void EditorUISystem::DrawTileSelectionPanel()
    {
        const ImVec2 screenSize = imgui_io->DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screenSize.x * 0.7f, 0.f));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.3f, screenSize.y));
        styleWindow();
        ImGui::PushStyleColor(ImGuiCol_Text, UINeonColors::windowBgColor);
        ImGui::PushFont(ui::FontManager::getFont("PixeloidSans-Bold"));
        ImGui::Begin("Tile Panel", nullptr, flags);
        if (ImGui::IsWindowHovered())
        {
            is_mouse_in_grid = false;
        }
        else
        {
            is_mouse_in_grid = true;
        }
        if (selected_grid_cells.empty())
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Click on grid to select position!");
        }
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::PushFont(ui::FontManager::getFont("PixeloidSans"));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));

        ImGui::Text("Soundtrack End:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%d  beats", static_cast<int>(final_beat_position));

        if (ImGui::Button("Save Level"))
        {
            levelLoading::LevelManager::saveCurrentLevel();
        }
        if (!selected_grid_cells.empty() && selected_group_cells.empty()) //don't allow deleting during active grouping
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete Selected Element"))
            {
                deleteAllAtSelectedPosition();
            }
        }

        ImGui::Separator();

        //handle multiselect and grouping
        bool pushed = false;
        if (multi_select_enabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, UINeonColors::Cyan);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UINeonColors::pastelNeonViolet2);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, UINeonColors::pastelNeonViolet);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.23f, 0.23f, 0.23f, 1.0f));
            pushed = true;
        }
        if (ImGui::Button(multi_select_enabled ? "Multi Select" : "Single Select"))
        {
            multi_select_enabled = !multi_select_enabled;
            if (!multi_select_enabled)
            {
                if (!selected_group_cells.empty())ecs::EventDispatcher::dispatcher.trigger(ui::EditorCancelGrouping{});
                //generate group, if active group selection already exists, but multi select is exited without hitting generate group
                selected_group_cells.clear();
                selected_grid_cells.clear();
                compute_group_AABB = false;
            }
        }
        if (pushed)
        {
            ImGui::PopStyleColor(4);
        }
        if (ImGui::IsItemHovered())
        {
            const std::string selectType = !multi_select_enabled ? "Multi Select" : "Single Select";
            ImGui::SetTooltip(("Switch to " + selectType).c_str());
        }
        if (multi_select_enabled)
        {
            if (ImGui::RadioButton("Generate Group Physics Collider", compute_group_AABB))
            {
                compute_group_AABB = !compute_group_AABB;
                if (!selected_group_cells.empty())ecs::EventDispatcher::dispatcher.trigger(ui::EditorCancelGrouping{});
                selected_grid_cells.clear();
                selected_group_cells.clear();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Tracks all Tiles placed until Generate Group");
            }
            if (compute_group_AABB)
            {
                if (ImGui::Button("Generate Group"))
                {
                    if (!selected_group_cells.empty())
                    {
                        ecs::EventDispatcher::dispatcher.trigger(ui::EditorGenerateGroup{});
                        selected_grid_cells.clear();
                        selected_group_cells.clear();
                    }
                }
            }
        }
        ImGui::Separator();

        ImGui::Text("Position Offset from Cell Center:");
        const auto xtItemWidth = ImGui::GetContentRegionAvail().x * 0.3f;
        ImGui::SetNextItemWidth(xtItemWidth);
        ImGui::InputFloat("offsetX", &selected_position_offset.x, 0.1f, 10.f, "%.2f");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(xtItemWidth);
        ImGui::InputFloat("offsetY", &selected_position_offset.y, 0.1f, 10.f, "%.2f");
        ImGui::Separator();

        ImGui::Text("Select shape:");
        if (ImGui::RadioButton("Rectangle", !is_triangle))
            is_triangle = false;
        ImGui::SameLine();
        if (ImGui::RadioButton("Triangle", is_triangle))
            is_triangle = true;
        ImGui::Separator();

        ImGui::Text("Scale:");
        const auto itemWidth = ImGui::GetContentRegionAvail().x * 0.3f;
        ImGui::SetNextItemWidth(itemWidth);
        if (ImGui::InputFloat("X", &selected_scale.x, 0.1f, 1.0f, "%.2f"))
        {
            if (selected_scale.x < 0.0f)
                selected_scale.x = 0.1f;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(itemWidth);
        if (ImGui::InputFloat("Y", &selected_scale.y, 0.1f, 10.f, "%.2f"))
        {
            if (selected_scale.x < 0.0f)
                selected_scale.x = 0.1f;
        }
        ImGui::Separator();

        ImGui::Text("Select tag:");
        const std::vector<std::string> tagButtonIDs{"platform", "obstacle"};
        highlightSelectedButton(tagButtonIDs);
        ImGui::Text("Custom tag:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.2f);
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
        ImGui::Separator();

        ImGui::Text("Z-Rotation:");
        const auto nextItemWidth = ImGui::GetContentRegionAvail().x * 0.3f;
        ImGui::SetNextItemWidth(nextItemWidth);
        ImGui::InputFloat("##zRot", &selected_z_rotation, 0.1f, 1.0f, "%.2f");
        selected_z_rotation = fmod(selected_z_rotation, 360.0f);
        if (selected_z_rotation < 0.0f)
            selected_z_rotation += 360.0f;
        ImGui::Separator();

        ImGui::Text("Render Layer:");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
        if (ImGui::InputFloat("Z-Layer", &selected_layer, 0.1f, 1.0f, "%.2f"))
        {
            if (selected_layer < -10.0f) // Constrain layer range (optional)
                selected_layer = -10.f;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip(
                "Negative values are further away from the camera \n (mostly used for rendering transparent objects first)");
        }
        ImGui::Separator();

        ImGui::Text("Generate PhysicsComponent");
        if (!compute_group_AABB)ImGui::Checkbox("##PhysicsComp", &generate_physics_comp);
        ImGui::Separator();

        const float availableWidth = ImGui::GetContentRegionAvail().x;
        const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float totalSpacing = itemSpacing * (tilesPerRow + 2);
        const float tileSize = (availableWidth - totalSpacing) / tilesPerRow;
        ImGui::Text("Select Visual");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "(Final Step)");

        if (ImGui::RadioButton("##Color", use_color))
        {
            use_color = true;
        }
        ImGui::SameLine();

        if (use_color)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().FramePadding.y);
            ImGui::ColorButton("##ColorPreview",
                               {selected_color.x, selected_color.y, selected_color.z, selected_color.w},
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

        if (ImGui::RadioButton("Texture", !use_color))
        {
            use_color = false;
        }

        if (use_color)
        {
            static bool pickerWasOpen = false;

            if (ImGui::BeginPopup("ColorPickerPopup"))
            {
                pickerWasOpen = true;

                ImGui::ColorPicker4("##picker", reinterpret_cast<float*>(&selected_color));

                ImGui::EndPopup();
            }
            else if (pickerWasOpen && !selected_grid_cells.empty())
            {
                pickerWasOpen = false;

                for (const auto& cell : selected_grid_cells)
                {
                    selected_color.r = std::round(selected_color.r * 100.0f) / 100.0f;
                    selected_color.g = std::round(selected_color.g * 100.0f) / 100.0f;
                    selected_color.b = std::round(selected_color.b * 100.0f) / 100.0f;

                    GameObject object = {
                        {
                            cell.x + selected_position_offset.x, cell.y + selected_position_offset.y, 0.f
                        },
                        selected_color,
                        selected_tag, is_triangle,
                        "", {selected_scale.x, selected_scale.y, 0.f}, {0, 0, 1, 1}, selected_z_rotation,
                        generate_physics_comp
                    };
                    object.zLayer = selected_layer;
                    ecs::EventDispatcher::dispatcher.trigger(ui::EditorTileSelectedEvent{object, compute_group_AABB});
                    if (compute_group_AABB)
                    {
                        selected_group_cells.push_back(cell);
                    }
                }
                selected_grid_cells.clear();
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

        ImGui::PopStyleColor(11);
        ImGui::PopStyleVar(3);
        ImGui::PopFont();
        ImGui::End();
    }

    void EditorUISystem::createCustomUI()
    {
        DrawTileSelectionPanel();
        drawGrid(pixelsPerMeter);
    }

    void EditorUISystem::update(const float deltaTime)
    {
        if (!game.isPaused()) return;
        createCustomUI();
    }
}
