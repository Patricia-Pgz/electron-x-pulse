#include "LevelPlayState.h"

#include "engine/ecs/EntityFactory.h"
#include "engine/levelloading/LevelLoader.h"

namespace gl3::game::state
{
    void LevelPlayState::loadLevel()
    {
        current_level_ = engine::levelLoading::LevelLoader::loadLevelByID(level_index_);

        for (auto& object : current_level_->objects)
        {
            engine::ecs::EntityFactory::createDefaultEntity(object, game_.getRegistry(), game_.getPhysicsWorld());
        }
    }

    void LevelPlayState::startLevel()
    {
        //TODO
    }

    void LevelPlayState::reloadLevel() //TODO
    {
    }
}
