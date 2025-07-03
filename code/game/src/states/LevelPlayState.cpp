#include "LevelPlayState.h"
#include "engine/Constants.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/levelloading/LevelLoader.h"
#include "ui/FinishUI.h"
#include "ui/InstructionUI.h"
#include <box2d/box2d.h>

#include "Game.h"
#include "../../../extern/box2d/src/body.h"
#include "engine/physics/PhysicsSystem.h"

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

    void LevelPlayState::applyBackgroundEntityTransform(LevelBackgroundConfig& bgConfig,
                                                        const entt::entity entity) const
    {
        auto& registry = game_.getRegistry();
        if (!registry.valid(entity)) return;
        if (const auto& tag = registry.get<engine::ecs::TagComponent>(entity).tag; tag == "ground")
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

    void LevelPlayState::updateBackgroundEntity(LevelBackgroundConfig& bgConfig, entt::entity entity) const
    {
        applyBackgroundEntityTransform(bgConfig, entity);
    }

    void LevelPlayState::onWindowResize(const engine::context::WindowResizeEvent& event) const
    {
        if (!level_instantiated_ || event.newHeight <= 0 || event.newWidth <= 0) return;

        auto& registry = game_.getRegistry();
        const auto view = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent,
                                        engine::ecs::PhysicsComponent>();

        auto bgConfig = calculateBackgrounds();
        for (auto& entity : view)
        {
            if (!registry.valid(entity)) continue;
            if (registry.get<engine::ecs::TagComponent>(entity).tag == "ground" || registry.get<
                engine::ecs::TagComponent>(entity).tag == "background")
            {
                updateBackgroundEntity(bgConfig, entity);
            }
        }
    }

    void LevelPlayState::loadLevel()
    {
        auto& registry = game_.getRegistry();
        const auto physicsWorld = game_.getPhysicsWorld();
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

            engine::ecs::EntityFactory::createDefaultEntity(
                object, registry, physicsWorld);
        }

        float initialPlayerPosX = 0.f;
        for (auto& object : current_level_->objects)
        {
            const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                object, registry, physicsWorld);
            if (object.tag == "player") current_player_ = entity;
            initialPlayerPosX = object.position.x;
            game_.setPlayer(current_player_);
        }
        game_.getAudioSystem()->initializeCurrentAudio(current_level_->audioFile, initialPlayerPosX);
        audio_config_ = game_.getAudioSystem()->getConfig();
        current_level_->currentLevelSpeed = current_level_->velocityMultiplier / audio_config_->seconds_per_beat;
        current_level_->levelLength = audio_config_->current_audio_length * current_level_->currentLevelSpeed;
        game_.getContext().setClearColor(current_level_->clearColor);

        level_instantiated_ = true;
        if (!edit_mode_)
        {
            game_.getAudioSystem()->playCurrentAudio();
            pauseOrStartLevel(false);
            return;
        }
        instruction_ui_->pauseTimer(true);
    }

    void LevelPlayState::moveObjects() const
    {
        for (const auto view = game_.getRegistry().view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();
             auto& entity : view)
        {
            if(!game_.getRegistry().valid(entity) || entity == entt::null)return;
            auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            if (auto& tag = view.get<engine::ecs::TagComponent>(entity).tag; tag == "platform" || tag == "obstacle")
            {
                b2Body_SetLinearVelocity(physics_comp.body, {current_level_->currentLevelSpeed * -1, 0.0f});
            }
        }
    }

    void LevelPlayState::stopMovingObjects() const
    {
        for (const auto view = game_.getRegistry().view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();
             auto& entity : view)
        {
            if(!game_.getRegistry().valid(entity) || entity == entt::null)return;
            auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            if (auto& tag = view.get<engine::ecs::TagComponent>(entity).tag; tag == "platform" || tag == "obstacle")
            {
                b2Body_SetLinearVelocity(physics_comp.body, {0.f, 0.0f});
            }
        }
    }

    void LevelPlayState::pauseOrStartLevel(const bool pause) const
    {
        auto* PlayerInputSystem = dynamic_cast<Game&>(game_).getPlayerInputSystem();
        if (pause)
        {
            game_.getPhysicsSystem()->setActive(false);
            PlayerInputSystem->setActive(false);
            audio_config_->audio.setPause(audio_config_->currentAudioHandle, true);
            stopMovingObjects();
            instruction_ui_->pauseTimer(true);
        }
        else
        {
            game_.getPhysicsSystem()->setActive(true);
            PlayerInputSystem->setActive(true);
            moveObjects();
            audio_config_->audio.setPause(audio_config_->currentAudioHandle, false);
            instruction_ui_->pauseTimer(false);
        }
    }

    void LevelPlayState::onPauseEvent(const engine::ui::PauseLevelEvent& event) const
    {
        pauseOrStartLevel(event.pauseLevel);
    }

    void LevelPlayState::onPlayerDeath(const engine::ecs::PlayerDeath& event)
    {
        //TODO game_.getAudioSystem()->playOneShot("crash");
        onRestartLevel();
    }

    void LevelPlayState::onRestartLevel()
    {
        //game will be reset and stopped if player restarts level and then presses enter in edit mode
        if (edit_mode_) play_test_ = true;
        reloadLevel();
        startLevel();
    }

    void LevelPlayState::startLevel() const
    {
        game_.getAudioSystem()->playCurrentAudio();
        pauseOrStartLevel(false);
    }

    /**
     *Resets every entity to its initial Transform, resets audio
*/
    void LevelPlayState::reloadLevel() //TODO gets called twice???
    {
        menu_ui_->setActive(true);
        instruction_ui_->setActive(level_index_ == 0);
        finish_ui_->setActive(false);

        game_.getAudioSystem()->stopCurrentAudio();

        timer_ = 1.f;
        transition_triggered_ = false;
        timer_active_ = false;


        auto& registry = game_.getRegistry();
        const auto view = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent,
                                        engine::ecs::PhysicsComponent>();

        auto bgConfig = calculateBackgrounds();
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
                updateBackgroundEntity(bgConfig, entity);
            }
        }
    }

    void LevelPlayState::delayLevelEnd(float deltaTime)
    {
        const auto currentTime = static_cast<float>(audio_config_->audio.getStreamTime(
            audio_config_->currentAudioHandle));

        if (!timer_active_ && currentTime >= audio_config_->current_audio_length - 1) //slight
        {
            timer_active_ = true;
        }

        if (timer_active_)
        {
            timer_ -= deltaTime;

            if (timer_ <= 0.0f && !transition_triggered_)
            {
                transition_triggered_ = true;

                menu_ui_->setActive(false);
                instruction_ui_->setActive(false);
                finish_ui_->setActive(true);

                //game_.getAudioSystem()->playOneShot("win"); TODO git sound pushen

                pauseOrStartLevel(true);
            }
        }
    }


    void LevelPlayState::unloadLevel()
    {
        level_instantiated_ = false;
        game_.getAudioSystem()->stopCurrentAudio();
        game_.getAudioSystem()->stopAllOneShots();
        menu_ui_->setActive(false);
        instruction_ui_->setActive(false);
        finish_ui_->setActive(false);
        menu_ui_ = nullptr;
        instruction_ui_ = nullptr;
        finish_ui_ = nullptr;

        engine::ecs::EntityFactory::clearRegistry(game_.getRegistry());
        level_index_ = -1;
        current_level_ = nullptr;
        current_player_ = entt::null;
        edit_mode_ = false;
        play_test_ = false;
    }

    void LevelPlayState::update(const float deltaTime)
    {
        if (!level_instantiated_)
        {
            return;
        }
        delayLevelEnd(deltaTime);
        if(!edit_mode_)return;
        if (glfwGetKey(game_.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) //TODO evtl in editstate enter abfragen
        {
            if (!enter_pressed_)
            {
                enter_pressed_ = true;
                play_test_ = !play_test_;
                engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::PlayModeChange{play_test_});
                game_.getContext().setCameraPos({0.0f, 0.0f, 1.0f});
                game_.getContext().setCameraCenter({0.f, 0.f, 0.f});
                if (play_test_)
                {
                    game_.getAudioSystem()->playCurrentAudio();
                    instruction_ui_->resetTimer();
                    pauseOrStartLevel(false);
                }
                else
                {
                    game_.getAudioSystem()->stopCurrentAudio();
                    pauseOrStartLevel(true);
                    reloadLevel();
                }
            }
        }
        else if (glfwGetKey(game_.getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        {
            enter_pressed_ = false;
        }
    }
}
