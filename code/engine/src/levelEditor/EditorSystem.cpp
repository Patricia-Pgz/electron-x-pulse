#include "engine/levelEditor/EditorSystem.h"

namespace gl3::engine::editor
{
    void EditorSystem::onTileSelected(TileSelectedEvent& event) const
    {
        std::cout << "JA";

        const auto& entity = ecs::EntityFactory::createDefaultEntity(game.getRegistry(),
                                                                     {
                                                                         event.selectedPixelPositionScreen.x,
                                                                         event.selectedPixelPositionScreen.y, 0.f
                                                                     },
                                                                     glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                                     event.selectedTag,
                                                                     game.getPhysicsWorld(), event.isTriangle,
                                                                     event.texture, event.uv);
        ecs::EntityFactory::setScale(game.getRegistry(), entity, {event.selectedScale.x, event.selectedScale.y, 0.f});
    }
} // gl3
