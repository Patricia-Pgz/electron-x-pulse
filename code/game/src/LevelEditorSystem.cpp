#include "LevelEditorSystem.h"

#include <iostream>
#include <engine/rendering/MVPMatrixHelper.h>

namespace gl3
{
    void LevelEditorSystem::onTileSelected(TileSelectedEvent& event)
    {
        const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(game.getRegistry(), {event.selectedPixelPositionScreen.x, event.selectedPixelPositionScreen.y, 0.f},
                                                        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), event.selectedTag,
                                                        game.getPhysicsWorld(), event.isTriangle, event.texture, event.uv);
        engine::ecs::EntityFactory::setScale(game.getRegistry(), entity, {event.selectedScale.x, event.selectedScale.y, 0.f});
    }
} // gl3
