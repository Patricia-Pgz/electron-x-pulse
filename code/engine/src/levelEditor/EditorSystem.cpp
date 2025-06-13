#include "engine/levelEditor/EditorSystem.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "engine/ecs/EntityFactory.h"

namespace gl3::engine::editor
{
    void EditorSystem::onTileSelected(TileSelectedEvent& event) const
    {
        const auto& entity = ecs::EntityFactory::createDefaultEntity(event.object, game_.getRegistry(),
                                                                     game_.getPhysicsWorld());
        // TODO wieder: ecs::EntityFactory::setScale(game_.getRegistry(), entity, {event.selectedScale.x, event.selectedScale.y, 0.f});
    }
} // gl3
