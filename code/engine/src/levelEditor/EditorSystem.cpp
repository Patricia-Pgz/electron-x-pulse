#include "engine/levelEditor/EditorSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/levelloading/LevelManager.h"
#include "engine/physics/PhysicsSystem.h"

namespace gl3::engine::editor
{
    EditorSystem::EditorSystem(Game& game): game(game)
    {
        ecs::EventDispatcher::dispatcher.sink<ui::EditorTileSelectedEvent>().connect<&
            EditorSystem::onTileSelected>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::FinalizeGroup>().connect<&
            EditorSystem::onFinalizeGroup>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::CancelGrouping>().connect<&
            EditorSystem::onGroupCanceled>(this);
    }

    EditorSystem::~EditorSystem()
    {
        ecs::EventDispatcher::dispatcher.sink<ui::EditorTileSelectedEvent>().disconnect<&
            EditorSystem::onTileSelected>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::FinalizeGroup>().disconnect<&
            EditorSystem::onFinalizeGroup>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::CancelGrouping>().disconnect<&
            EditorSystem::onGroupCanceled>(this);
    }

    void EditorSystem::onTileSelected(ui::EditorTileSelectedEvent& event)
    {
        auto& reg = game.getRegistry();
        const b2WorldId physicsWorld = game.getPhysicsWorld();

        // Avoid creating physics for grouped tiles individually
        event.object.generatePhysicsComp = event.group ? false : event.object.generatePhysicsComp;

        // Create the visual/game entity
        const auto entity = ecs::EntityFactory::createDefaultEntity(event.object, reg, physicsWorld);

        if (event.group)
        {
            // If no current group body exists, create a parent body and entity
            if (B2_ID_EQUALS(current_parent_body_id, b2_nullBodyId))
            {
                current_group.parent = event.object;
                current_group.parent.textureName = "";
                current_group.parent.isSensor = true;
                current_group.parent.generatePhysicsComp = true;
                current_group.parent.generateRenderComp = false;
                current_parent_entity = ecs::EntityFactory::createDefaultEntity(
                    current_group.parent, reg, physicsWorld);
                current_parent_body_id = reg.get<ecs::PhysicsComponent>(current_parent_entity).body;
                reg.emplace<ecs::PhysicsGroupParent>(current_parent_entity, current_parent_body_id, 0);
            }
            // Compute local offset relative to parent body
            const auto parentPos = reg.get<ecs::TransformComponent>(current_parent_entity).position;
            glm::vec2 localOffset = {
                event.object.position.x - parentPos.x,
                event.object.position.y - parentPos.y
            };

            // Create a shape for this child tile
            const b2ShapeDef shapeDef = b2DefaultShapeDef();
            const b2Polygon polygon = b2MakeOffsetBox(
                event.object.scale.x * 0.5f,
                event.object.scale.y * 0.5f,
                {localOffset.x, localOffset.y},
                b2MakeRot(event.object.zRotation)
            );
            b2ShapeId shapeId = b2CreatePolygonShape(current_parent_body_id, &shapeDef, &polygon);

            // Attach ECS PhysicsGroup linking child to parent
            reg.emplace<ecs::PhysicsGroupChild>(entity, current_parent_entity, localOffset, shapeId);

            // Increment the parent’s child count
            reg.patch<ecs::PhysicsGroupParent>(current_parent_entity, [](auto& pgp) { ++pgp.childCount;  ++pgp.visibleChildren;});
            current_group.children.push_back(event.object);
        }
        else
        {
            levelLoading::LevelManager::addObjectToCurrentLevel(event.object);
        }
        ecs::EventDispatcher::dispatcher.trigger(ecs::RenderComponentContainerChange{});
    }


    void EditorSystem::onFinalizeGroup(const ui::FinalizeGroup& event)
    {
        levelLoading::LevelManager::addGroupToCurrentLevel(current_group);
        current_group = {};
        current_parent_body_id = b2_nullBodyId;
        current_parent_entity = entt::null;
    }

    void EditorSystem::onGroupCanceled(const ui::CancelGrouping& event)
    {
        current_group = {};
        current_parent_body_id = b2_nullBodyId;
        current_parent_entity = entt::null;
    }
} // gl3
