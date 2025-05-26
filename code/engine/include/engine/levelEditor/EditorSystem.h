#pragma once
#include "engine/ecs/EventDispatcher.h"
#include "engine/Game.h"

namespace gl3::engine::editor
{
    struct TileSelectedEvent;

    class EditorSystem
    {
    public:
        explicit EditorSystem(Game& game): game_(game)
        {
            ecs::EventDispatcher::dispatcher.sink<TileSelectedEvent>().connect<&
                EditorSystem::onTileSelected>(this);
        }
        ;

        ~EditorSystem()
        {
            ecs::EventDispatcher::dispatcher.sink<TileSelectedEvent>().disconnect<&
                EditorSystem::onTileSelected>(this);
        };

    private:
        Game& game_;
        void onTileSelected(TileSelectedEvent& event) const;
    };
} // gl3
