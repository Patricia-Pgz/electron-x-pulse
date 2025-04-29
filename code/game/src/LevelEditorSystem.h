#pragma once
#include "GameUISystem.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/System.h"

namespace gl3
{
    class LevelEditorSystem : public engine::ecs::System
    {
    public:
        explicit LevelEditorSystem(engine::Game& game) : System(game)
        {
            engine::ecs::EventDispatcher::dispatcher.sink<TileSelectedEvent>().connect<&
                LevelEditorSystem::onTileSelected>(this);
        };

        ~LevelEditorSystem() override
        {
            engine::ecs::EventDispatcher::dispatcher.sink<TileSelectedEvent>().disconnect<&
                LevelEditorSystem::onTileSelected>(this);
        };

    private:
        void onTileSelected(TileSelectedEvent& event);
    };
} // gl3
