#include "LevelPlayState.h"

#include "engine/Constants.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/levelloading/LevelLoader.h"

namespace gl3::game::state
{
    /**
     *@short Calculates the scales and positions for the two horizontally divided background visuals.
*The Background is divided horizontally by the groundlevel. In the upper portion, objects tagged "background" get layered on top of each other,
*in the order in which they appear in the level's .json file @see LevelPlayState::loadLevel()
*Same for the bottom part, which layers objects tagged as "ground".
*@warning This does not keep aspect ratios of textures, it just fits the object sizes and positions to the screen. May stretch textures!
*/
    LevelBackgroundConfig LevelPlayState::calculateBackgrounds() const
    {
        const auto windowBounds = game_.getContext().getWindowBounds(); // pixels: left, right, bottom, top

        float left_m = windowBounds[0] / pixelsPerMeter;
        float right_m = windowBounds[1] / pixelsPerMeter;
        float bottom_m = windowBounds[2] / pixelsPerMeter;
        float top_m = windowBounds[3] / pixelsPerMeter;

        float center_x = (left_m + right_m) / 2.f;
        float windowWidth = right_m - left_m;

        float groundLevel = current_level_->groundLevel;

        float ground_center_y = (bottom_m + groundLevel) / 2.f;
        float ground_height = groundLevel - bottom_m;

        float sky_center_y = (groundLevel + top_m) / 2.f;
        float sky_height = top_m - groundLevel;

        return {center_x, windowWidth, ground_center_y, ground_height, sky_center_y, sky_height};
    }

    void LevelPlayState::onWindowResize(engine::context::WindowResizeEvent& event) const
    {
        if (!isLevelInstantiated || event.newHeight <= 0 || event.newWidth <= 0) return;
        auto& registry = game_.getRegistry();
        const auto view = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent,
                                        engine::ecs::PhysicsComponent>();
        auto bgConfig = calculateBackgrounds();

        for (auto entity : view)
        {
            auto& transform = view.get<engine::ecs::TransformComponent>(entity);
            auto& tag = view.get<engine::ecs::TagComponent>(entity).tag;
            auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);

            if (tag == "ground")
            {
                engine::ecs::EntityFactory::setPosition(registry, entity, {
                                                            bgConfig.center_x, bgConfig.ground_center_y, 0.f
                                                        });
                engine::ecs::EntityFactory::setScale(registry, entity, {
                                                         bgConfig.windowWidth, bgConfig.ground_height, 1.f
                                                     });
            }
            else if (tag == "background")
            {
                engine::ecs::EntityFactory::setPosition(registry, entity, {
                                                            bgConfig.center_x, bgConfig.sky_center_y, 0.f
                                                        });
                engine::ecs::EntityFactory::setScale(registry, entity, {
                                                         bgConfig.windowWidth, bgConfig.sky_height, 1.f
                                                     });
            }
        }
    }

    void LevelPlayState::loadLevel()
    {
        current_level_ = engine::levelLoading::LevelLoader::loadLevelByID(level_index_);
        game_.getAudioSystem().initializeCurrentAudio(current_level_->audioFile);

        const auto bgConfig = calculateBackgrounds();
        for (auto& object : current_level_->backgrounds)
        {
            if (object.tag == "ground")
            {
                object.position = {bgConfig.center_x, bgConfig.ground_center_y, 0.f};
                object.scale = {bgConfig.windowWidth, bgConfig.ground_height, 1.f};
            }
            else
            {
                object.position = {bgConfig.center_x, bgConfig.sky_center_y, 0.f};
                object.scale = {bgConfig.windowWidth, bgConfig.sky_height, 1.f};
            }

            engine::ecs::EntityFactory::createDefaultEntity(
                object, game_.getRegistry(), game_.getPhysicsWorld());
        }

        for (auto& object : current_level_->objects)
        {
            const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                object, game_.getRegistry(), game_.getPhysicsWorld());
            if (object.tag == "player") current_player_ = entity;
        }
        isLevelInstantiated = true;
        startLevel();
    }

    void LevelPlayState::startLevel()
    {
        //TODO evtl in audiosystem statt hier (wenn musik ready)
        engine::ecs::EventDispatcher::dispatcher.trigger<engine::ecs::LevelStartEvent>({current_player_});
    }

    void LevelPlayState::reloadLevel() //TODO
    {
    }

    void LevelPlayState::unloadLevel()
    {
        isLevelInstantiated = false;
        //TODO Level von Game trennen?
        //TODO player in game auf null setzen + entites aus registry löschen + musik stoppen + schauen ob wirklich alles nötige gelöscht wurde!
    }
}
