#pragma once
#include "EditorUISystem.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/System.h"
#include "engine/Game.h"

namespace gl3::engine::editor
{
    class EditorSystem : public ecs::System
    {
    public:
        explicit EditorSystem(Game& game) : System(game)
        {
            ecs::EventDispatcher::dispatcher.sink<TileSelectedEvent>().connect<&
                EditorSystem::onTileSelected>(this);
        };

        ~EditorSystem() override
        {
            ecs::EventDispatcher::dispatcher.sink<TileSelectedEvent>().disconnect<&
                EditorSystem::onTileSelected>(this);
        };

    private:
        void onTileSelected(TileSelectedEvent& event) const;
    };
} // gl3
