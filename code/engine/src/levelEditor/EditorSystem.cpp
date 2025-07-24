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
        ecs::EventDispatcher::dispatcher.sink<ui::EditorGenerateGroup>().connect<&
            EditorSystem::onGenerateGroup>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::EditorGroupTileDeleted>().connect<&
            EditorSystem::onGroupTileDeleted>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::EditorCancelGrouping>().connect<&
    EditorSystem::onGroupCanceled>(this);
    }

    EditorSystem::~EditorSystem()
    {
        ecs::EventDispatcher::dispatcher.sink<ui::EditorTileSelectedEvent>().disconnect<&
            EditorSystem::onTileSelected>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::EditorGenerateGroup>().disconnect<&
            EditorSystem::onGenerateGroup>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::EditorGroupTileDeleted>().disconnect<&
            EditorSystem::onGroupTileDeleted>(this);
        ecs::EventDispatcher::dispatcher.sink<ui::EditorCancelGrouping>().disconnect<&
EditorSystem::onGroupCanceled>(this);
    }

    void EditorSystem::onTileSelected(ui::EditorTileSelectedEvent& event)
    {
        event.object.generatePhysicsComp = event.group ? false : event.object.generatePhysicsComp;
        const auto entity = ecs::EntityFactory::createDefaultEntity(event.object, game.getRegistry(),
                                                                    game.getPhysicsWorld());
        //save children for later grouping
        if (event.group)
        {
            grouped_child_entities.push_back(entity);
            grouped_child_objects.push_back(event.object);
            return;
        }
        levelLoading::LevelManager::addObjectToCurrentLevel(event.object);
        ecs::EventDispatcher::dispatcher.trigger(ecs::RenderComponentContainerChange{});
    }

    void EditorSystem::onGroupTileDeleted(const ui::EditorGroupTileDeleted& event)
    {
        if (grouped_child_entities.empty() || grouped_child_objects.empty())return;
        //Delete the child GameObjects and entities, that are at the selected positions.
        for (const auto& cell : event.selectedCells)
        {
            std::erase_if(grouped_child_objects,
                          [&](const GameObject& obj)
                          {
                              return obj.position.x == cell.x && obj.position.y == cell.y;
                          });
            std::erase_if(grouped_child_entities,
                          [&](auto& entity)
                          {
                              const auto transform = game.getRegistry().get<ecs::TransformComponent>(entity);
                              if (transform.position.x == cell.x && transform.position.y == cell.y)
                              {
                                  return true;
                              }
                                  return false;
                          });
        }
    }

    void EditorSystem::onGenerateGroup(ui::EditorGenerateGroup& event)
    {
        if (grouped_child_entities.empty() || grouped_child_objects.empty())return;
        auto& registry = game.getRegistry();

        //generate parent physics AABB
        auto parentAABB = physics::PhysicsSystem::computeGroupAABB(grouped_child_objects);
        parentAABB.zLayer = -1;
        parentAABB.generateRenderComp = false;
        parentAABB.tag = grouped_child_objects[0].tag;
        entt::entity parentAABBEntity = ecs::EntityFactory::createDefaultEntity(
            parentAABB, registry, game.getPhysicsWorld());

        //Set children localOffsets and ParentComponent & parent's GroupComponent
        for (auto& entity : grouped_child_entities)
        {
            auto& transform = registry.get<ecs::TransformComponent>(entity);
            glm::vec2 localOffset = {
                transform.position.x - parentAABB.position.x, transform.position.y - parentAABB.position.y
            };
            registry.emplace<ecs::ParentComponent>(entity, parentAABBEntity, localOffset);
        }
        registry.emplace<ecs::GroupComponent>(parentAABBEntity, grouped_child_entities);

        //add group to currentLvl (for saving)
        int groupCount = static_cast<int>(levelLoading::LevelManager::getMostRecentLoadedLevel()->groups.size()) + 1;
        GameObjectGroup group;
        group.name = std::to_string(groupCount);
        group.children = grouped_child_objects;
        group.colliderAABB = parentAABB;
        levelLoading::LevelManager::addGroupToCurrentLevel(group);

        grouped_child_entities.clear();
        grouped_child_objects.clear();
    }

    void EditorSystem::onGroupCanceled(ui::EditorCancelGrouping& event)
    {
        grouped_child_entities.clear();
        grouped_child_objects.clear();
    }
} // gl3
