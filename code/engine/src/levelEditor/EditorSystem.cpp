#include "engine/levelEditor/EditorSystem.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "engine/ecs/EntityFactory.h"

namespace gl3::engine::editor
{
    void EditorSystem::onTileSelected(TileSelectedEvent& event) const
    {
        ecs::EntityFactory::createDefaultEntity(event.object, game_.getRegistry(),
                                                game_.getPhysicsWorld());
    }
} // gl3
