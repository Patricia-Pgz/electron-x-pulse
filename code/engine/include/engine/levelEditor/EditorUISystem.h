/**
* @file EditorUISystem.h
 * @brief Defines the EditorUISystem, a UI subsystem for the level editor using ImGui.
 */
#pragma once
#include "EditorSystem.h"
#include "engine/Constants.h"
#include "engine/rendering/Texture.h"
#include "engine/Game.h"
#include "engine/ecs/GameEvents.h"
#include "engine/levelloading/Objects.h"
#include "engine/userInterface/IUISubSystem.h"

namespace gl3::engine::editor
{
    /**
     * @brief Number of tiles per row used in tile selection UI.
     */
    constexpr int tilesPerRow = 4;

    /**
     * @class EditorUISystem
     * @brief UI subsystem managing editor interface elements and interactions.
     *
     * This system handles rendering of editor panels, tile selection,
     * grid visualization, input handling such as mouse scroll, and integration
     * with the EditorSystem for editing game levels and objects.
     *
     * It subscribes to mouse scroll and level computation events to update UI accordingly.
     */
    class EditorUISystem final : public ui::IUISubsystem
    {
    public:
        /**
         * @brief Constructs the EditorUISystem.
         * @param imguiIO Pointer to ImGui IO for input handling.
         * @param game Reference to the game engine.
         *
         * Subscribes to mouse scroll, level length computed, and level unload events.
         */
        explicit EditorUISystem(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game),
                                                                editor_system(new EditorSystem(game))
        {
            grid_spacing = pixelsPerMeter * grid_spacing_factor;
            ecs::EventDispatcher::dispatcher.sink<context::MouseScrollEvent>().connect<&
                EditorUISystem::onMouseScroll>(this);
            ecs::EventDispatcher::dispatcher.sink<ecs::LevelLengthComputed>().connect<&
                EditorUISystem::onLvlComputed>(this);
            ecs::EventDispatcher::dispatcher.sink<ui::LevelUnload>().connect<&
                EditorUISystem::reset>(this);
        };

        ~EditorUISystem() override
        {
            ecs::EventDispatcher::dispatcher.sink<context::MouseScrollEvent>().disconnect<&
                EditorUISystem::onMouseScroll>(this);
            ecs::EventDispatcher::dispatcher.sink<ecs::LevelLengthComputed>().disconnect<&
                EditorUISystem::onLvlComputed>(this);
            ecs::EventDispatcher::dispatcher.sink<ui::LevelUnload>().disconnect<&
                EditorUISystem::reset>(this);
        }

        /**
         * @brief Updates the UI elements each frame, if is_active.
         * Overrides the base IUISubsystem update method.
         */
        void update(float deltaTime) override;

        /**
             * @brief Deletes all objects in the currently selected grid cell.
             */
        void deleteAllAtSelectedCell() const;

        /**
         * @brief Handles mouse scroll input event.
         * @param event The mouse scroll event data.
         */
        void onMouseScroll(const context::MouseScrollEvent& event) const;

        /**
         * @brief Handles the level length computed event, saves the final beat position for visualization.
         * @param event The level length computed event data.
         */
        void onLvlComputed(const ecs::LevelLengthComputed& event);

        /**
         * @brief Draws a grid visualization with specified spacing.
         */
        void drawGrid();

        /**
         * @brief Draws the tile selection panel UI.
         */
        void DrawTileSelectionPanel();

        /**
         * @brief Creates additional custom UI elements.
         */
        void createCustomUI();

        /**
         * @brief Visualizes a tileset texture in the UI.
         * @param texture The texture representing the tileset.
         * @param name Name/title of the tileset.
         * @param tileSize Size of each tile in the UI.
         */
        void visualizeTileSetUI(const rendering::Texture& texture, const std::string& name,
                                float tileSize);

        /**
         * @brief Visualizes a single texture in the UI.
         * @param texture The texture to display.
         * @param name Name of the texture.
         * @param tileSize Display size for the texture.
         */
        void visualizeSingleTextureUI(const rendering::Texture& texture, const std::string& name,
                                      float tileSize);

        /**
         * @brief Highlights selected buttons given their IDs.
         * @param buttonIDs Vector of button identifier strings.
         */
        void highlightSelectedButton(const std::vector<std::string>& buttonIDs);

    private:
        void reset();
        bool is_mouse_in_grid = true;
        /**< Whether the mouse is currently interacting with the grid vs. with the imgui ui. */
        bool multi_select_enabled = false; /**< Enables multi-selection mode. */
        bool is_grouping = false; /**< Flag to compute axis-aligned bounding box for groups. */
        ImVec2 grid_center = {0.f, 0.f};
        float grid_spacing; /**< The grid spacing for each cell in pixelspermeter */
        float grid_spacing_factor = 1.f; /**< The factor to apply to customize the grid spacing */
        float grid_offset = 0.5f; /**< Offset value for grid alignment. */
        std::vector<ImVec2> selected_grid_cells; /**< Currently selected grid cells in the editor. */
        std::vector<ImVec2> selected_group_cells; /**< Selected cells to group. */
        bool advancedSettings = false;
        /**< Flag to toggle advanced developer settings (changes made in advanced settings might only be reverse-able by editing the level json file). */
        glm::vec2 selected_position_offset = {0.f, 0.f}; /**< Offset from cell center. */
        glm::vec2 selected_scale = {1.f, 1.f};
        float selected_layer = 0.f; /**< Selected layer for rendering*/
        char tag_input_buffer[128] = ""; /**< Input buffer for tag text selection. */
        std::string selected_tag = "platform"; /**< Tag assigned to entities to create. */
        bool is_triangle = false; /**< Whether selected shape is a triangle. */
        float selected_z_rotation = 0.f; /**< Rotation angle around Z-axis for entity creation. */
        bool generate_physics_comp = true; /**< Whether to generate physics component when creating the object(s). */
        bool is_sensor = false; /**< Whether the physics body of this object is only a sensor */
        bool use_color = false; /**< Whether to apply custom coloring or texture to entity creation. */
        bool repeatTextureOnX = false; /**< Whether to repeat the texture on the x-axis. */
        glm::vec4 selected_color = {1.0f, 1.0f, 1.0f, 1.0f}; /**< Color used for entity creation. */
        EditorSystem* editor_system; /**< Pointer to the main editor system instance. */
        float final_beat_position = 0.f; /**< Position of final beat for timing music-synced editing. */

        static constexpr ImGuiWindowFlags flags = /**< ImGui window flags for the editor UI window. */
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize;
    };
}
