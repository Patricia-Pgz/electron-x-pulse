#pragma once
#include "engine/ecs/EventDispatcher.h"
#include "engine/Game.h"
#include "engine/levelLoading/Objects.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::engine::editor
{
    class EditorSystem
    {
    public:
        explicit EditorSystem(Game& game): game(game)
        {
            ecs::EventDispatcher::dispatcher.sink<ui::EditorTileSelectedEvent>().connect<&
                EditorSystem::onTileSelected>(this);
            ecs::EventDispatcher::dispatcher.sink<ui::EditorGenerateGroup>().connect<&
                EditorSystem::onGenerateGroup>(this);
        }
        ;

        ~EditorSystem()
        {
            ecs::EventDispatcher::dispatcher.sink<ui::EditorTileSelectedEvent>().disconnect<&
                EditorSystem::onTileSelected>(this);
            ecs::EventDispatcher::dispatcher.sink<ui::EditorGenerateGroup>().disconnect<&
                EditorSystem::onGenerateGroup>(this);
        };

    private:
        Game& game;
        std::vector<entt::entity> grouped_child_entities;
        std::vector<GameObject> grouped_child_objects;
        void onTileSelected(ui::EditorTileSelectedEvent& event);
        void onGenerateGroup(ui::EditorGenerateGroup& event);
    };
} // gl3
