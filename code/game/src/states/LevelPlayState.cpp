#include "LevelPlayState.h"

#include "engine/Constants.h"
#include "engine/audio/AudioSystem.h"
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

    void LevelPlayState::applyBackgroundEntityTransform(LevelBackgroundConfig& bgConfig) const
    {
        auto& registry = game_.getRegistry();
        for (auto& entity : background_entities_)
        {
            if (!registry.valid(entity)) continue;
            if (auto& tag = registry.get<engine::ecs::TagComponent>(entity).tag; tag == "ground")
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

    void LevelPlayState::updateBackgroundEntities() const
    {
        auto bgConfig = calculateBackgrounds();
        applyBackgroundEntityTransform(bgConfig);
    }

    void LevelPlayState::onWindowResize(const engine::context::WindowResizeEvent& event) const
    {
        if (!level_instantiated_ || event.newHeight <= 0 || event.newWidth <= 0) return;
        updateBackgroundEntities();
    }

    void LevelPlayState::loadLevel()
    {
        auto& registry = game_.getRegistry();
        auto physicsWorld = game_.getPhysicsWorld();
        current_level_ = engine::levelLoading::LevelLoader::loadLevelByID(level_index_);

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

            auto entity = engine::ecs::EntityFactory::createDefaultEntity(
                object, registry, physicsWorld);
            background_entities_.push_back(entity);
        }

        float initialPlayerPosX = 0.f;
        for (auto& object : current_level_->objects)
        {
            const auto entity = engine::ecs::EntityFactory::createDefaultEntity(
                object, registry, physicsWorld);
            if (object.tag == "player") current_player_ = entity;
            initialPlayerPosX = object.position.x;
        }
        audio_config_ = game_.getAudioSystem().initializeCurrentAudio(current_level_->audioFile, initialPlayerPosX);
        current_level_->currentLevelSpeed = current_level_->velocityMultiplier / audio_config_->seconds_per_beat;
        current_level_->levelLength = 5; //audio_config_->current_audio_length*levelspeed

        level_instantiated_ = true;
        startLevel();
    }

    void LevelPlayState::moveObjects() const
    {
        const auto view = game_.getRegistry().view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();
        for (auto& entity : view)
        {
            auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            auto& tag_comp = view.get<engine::ecs::TagComponent>(entity);
            if (tag_comp.tag == "platform" || tag_comp.tag == "obstacle")
            {
                b2Body_SetLinearVelocity(physics_comp.body, {current_level_->currentLevelSpeed * -1, 0.0f});
            }
        }
    }

    void LevelPlayState::stopMovingObjects() const
    {
        const auto view = game_.getRegistry().view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();
        for (auto& entity : view)
        {
            auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            auto& tag_comp = view.get<engine::ecs::TagComponent>(entity);
            if (tag_comp.tag == "platform" || tag_comp.tag == "obstacle")
            {
                b2Body_SetLinearVelocity(physics_comp.body, {0.f, 0.0f});
            }
        }
    }

    void LevelPlayState::startLevel() const
    {
        //TODO brauche ich das event noch? -> wenn nein sind start und resume das gleiche
        engine::ecs::EventDispatcher::dispatcher.trigger<engine::ecs::LevelStartEvent>({current_player_});

        moveObjects();
        game_.getAudioSystem().playCurrentAudio();
    }

    void LevelPlayState::pauseLevel() const //UI Event abfangen in InGameMenuUI -> ESC
    {
        game_.getAudioSystem().stopCurrentAudio();
        stopMovingObjects();
    }

    void LevelPlayState::resumeLevel() const //TODO UI event abfangen in InGameMenuUI
    {
        moveObjects();
        game_.getAudioSystem().playCurrentAudio();
    }


    /**
     *Resets every entity to its initial Transform and restarts movement and audio
*/
    void LevelPlayState::reloadLevel()
    //TODO zusätzlich UI event aus InGameMenuUI & FinishMenu abfangen! (restart lvl -> button hinzufügen)
    {
        game_.getAudioSystem().stopCurrentAudio();
        auto& registry = game_.getRegistry();
        const auto view = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent,
                                        engine::ecs::PhysicsComponent>();
        background_entities_.clear();
        for (auto& entity : view)
        {
            if (!registry.valid(entity)) continue;
            engine::ecs::EntityFactory::setPosition(registry, entity,
                                                    registry.get<engine::ecs::TransformComponent>(entity).
                                                             initialPosition);
            engine::ecs::EntityFactory::setScale(registry, entity,
                                                 registry.get<engine::ecs::TransformComponent>(entity).
                                                          initialScale);
            engine::ecs::EntityFactory::SetRotation(registry, entity,
                                                    registry.get<engine::ecs::TransformComponent>(entity).
                                                             initialZRotation);
            if (registry.get<engine::ecs::TagComponent>(entity).tag == "ground" || registry.get<
                engine::ecs::TagComponent>(entity).tag == "background")
            {
                background_entities_.push_back(entity);
                //TODO backgrounds hier direkt updaten, anstatt in array schieben und dann updaten!
            }
        }

        updateBackgroundEntities();

        game_.getAudioSystem().playCurrentAudio();
    }

    void LevelPlayState::delayLevelEnd(float deltaTime)
    {
        const float currentTime = audio_config_->audio.getStreamTime(audio_config_->currentAudioHandle);

        if (!timer_active_ && currentTime >= 5.0f)
        {
            timer_active_ = true;
        }

        if (timer_active_)
        {
            timer_ -= deltaTime;

            if (timer_ <= 0.0f && !transition_triggered_)
            {
                transition_triggered_ = true;

                menu_ui_.setActive(false);
                game_.getUISystem().getSubsystems(2).setActive(false);
                game_.getUISystem().getSubsystems(3).setActive(true);
                pauseLevel();
            }
        }
    }


    void LevelPlayState::unloadLevel()
    {
        level_instantiated_ = false;
        //TODO player in game auf null setzen + entites aus registry löschen + schauen ob wirklich alles nötige gelöscht wurde!
    }
}
