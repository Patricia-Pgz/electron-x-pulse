#include "engine/levelEditor/EditorSystem.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "engine/ecs/EntityFactory.h"

namespace gl3::engine::editor
{
    void EditorSystem::onTileSelected(TileSelectedEvent& event) const
    {
        const auto& entity = ecs::EntityFactory::createDefaultEntity(game_.getRegistry(),
                                                                     {
                                                                         event.selectedPixelPositionScreen.x,
                                                                         event.selectedPixelPositionScreen.y, 0.f
                                                                     },
                                                                     glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                                     event.selectedTag,
                                                                     game_.getPhysicsWorld(), event.isTriangle,
                                                                     event.texture, event.uv);
        ecs::EntityFactory::setScale(game_.getRegistry(), entity, {event.selectedScale.x, event.selectedScale.y, 0.f});
    }
} // gl3
