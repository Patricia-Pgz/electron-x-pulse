#include "LevelPlayState.h"
#include "engine/Constants.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/levelloading/LevelManager.h"
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

        const float left_m = windowBounds[0] / pixelsPerMeter;
        const float right_m = windowBounds[1] / pixelsPerMeter;
        const float bottom_m = windowBounds[2] / pixelsPerMeter;
        const float top_m = windowBounds[3] / pixelsPerMeter;

        const float center_x = (left_m + right_m) / 2.f;
        const float windowWidth = right_m - left_m;

        const float groundLevel = current_level_->groundLevel;

        constexpr float kMinHeight = 0.01f; // 1 cm, avoid 0 height
        const float ground_center_y = (bottom_m + groundLevel) / 2.f;
        const float ground_height = std::max(kMinHeight, groundLevel - bottom_m);
        const float sky_center_y = (groundLevel + top_m) / 2.f;
        const float sky_height = std::max(kMinHeight, top_m - groundLevel);

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
        current_level_ = engine::levelLoading::LevelManager::loadLevelByID(level_index_);

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

        for (auto& objGroup : current_level_->groups)
        {
            std::vector<entt::entity> children;
            //objGroup.colliderAABB.generateRenderComp = false;
            for (auto& obj : objGroup.children)
            {
                obj.generatePhysicsComp = false;
                glm::vec2 localOffset = {
                    obj.position.x - objGroup.colliderAABB.position.x, obj.position.y - objGroup.colliderAABB.position.y
                };
                const entt::entity entity = engine::ecs::EntityFactory::createDefaultEntity(
                    obj, registry, physicsWorld);
                registry.emplace<engine::ecs::Parent>(entity, nullptr, localOffset);
                children.push_back(entity);
            }

            if (objGroup.colliderAABB.scale.x <= 0.f || objGroup.colliderAABB.scale.y <= 0.f)
            {
                objGroup.colliderAABB = engine::physics::PhysicsSystem::computeGroupAABB(registry, children);
                if (objGroup.colliderAABB.scale.x <= 0)
                {
                    std::cerr << "Invalid bounding box size!";
                    return;
                }
            }

            entt::entity groupAABBEntity = engine::ecs::EntityFactory::createDefaultEntity(
                objGroup.colliderAABB, registry, physicsWorld);
            for (const auto& child : children)
            {
                registry.get<engine::ecs::Parent>(child).parentObject = &groupAABBEntity;
            }
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
        //Ensures, that every unit is synced to the beat
        current_level_->currentLevelSpeed = current_level_->velocityMultiplier / audio_config_->seconds_per_beat;
        current_level_->levelLength = audio_config_->current_audio_length * current_level_->currentLevelSpeed;
        current_level_->finalBeatIndex = audio_config_->current_audio_length / audio_config_->seconds_per_beat;
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::LevelLengthComputed{
            current_level_->levelLength, current_level_->currentLevelSpeed, current_level_->finalBeatIndex
        });

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
            if (!game_.getRegistry().valid(entity) || entity == entt::null)return;
            const auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
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
            if (!game_.getRegistry().valid(entity) || entity == entt::null)return;
            const auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            if (auto& tag = view.get<engine::ecs::TagComponent>(entity).tag; tag == "platform" || tag == "obstacle")
            {
                b2Body_SetLinearVelocity(physics_comp.body, {0.f, 0.0f});
            }
        }
    }

    void LevelPlayState::pauseOrStartLevel(const bool pause)
    {
        paused = pause;
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

    void LevelPlayState::onPauseEvent(const engine::ui::PauseLevelEvent& event)
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

    void LevelPlayState::startLevel()
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
        if (!paused)
        {
            engine::visual_effects::Parallax::moveBgObjectsParallax(
                game_.getRegistry(), deltaTime, current_level_->currentLevelSpeed);
        }
        if (!edit_mode_)return;
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
