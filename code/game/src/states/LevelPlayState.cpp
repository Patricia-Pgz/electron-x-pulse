#include "LevelPlayState.h"
#include "engine/audio/AudioSystem.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/levelloading/LevelManager.h"
#include "ui/FinishUI.h"
#include "ui/InstructionUI.h"
#include <box2d/box2d.h>
#include "../../../extern/box2d/src/body.h"

#include "Game.h"
#include "engine/physics/PhysicsSystem.h"

namespace gl3::game::state
{
    /**
     * Recompute the sizes of backround/ground entities -> they stay fixed to the center of their part of the screen (parted by current_level_->groundLevel)
     * @param event The event, that the Context sends, when the window size changes.
     */
    void LevelPlayState::onWindowSizeChange(const engine::context::WindowBoundsRecomputeEvent& event) const
    {
        const auto windowBounds = *event.windowBounds;
        const auto windowLeftWorld = windowBounds[0];
        const auto windowRightWorld = windowBounds[1];
        const auto windowTopWorld = windowBounds[2];
        const auto windowBottomWorld = windowBounds[3];

        const float center_x = (windowLeftWorld + windowRightWorld) * 0.5f;
        const float windowWidth = windowRightWorld - windowLeftWorld;

        const float groundLevel = current_level_->groundLevel;

        constexpr float kMinHeight = 0.01f; // 1 cm, avoid 0 height
        const float groundCenterY = (windowBottomWorld + groundLevel) / 2.f;
        const float groundHeight = std::max(kMinHeight, groundLevel - windowBottomWorld);
        const float skyCenterY = (groundLevel + windowTopWorld) / 2.f;
        const float skyHeight = std::max(kMinHeight, windowTopWorld - groundLevel);

        auto& registry = game_.getRegistry();
        const auto entities = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent>();

        for (auto entity : entities)
        {
            if (auto tag = entities.get<engine::ecs::TagComponent>(entity).tag; tag == "ground")
            {
                engine::ecs::EntityFactory::setPosition(registry, entity, {
                                                            center_x, groundCenterY, 0.f
                                                        });
                engine::ecs::EntityFactory::setScale(registry, entity, {
                                                         windowWidth, groundHeight, 1.f
                                                     });
            }
            else if (tag == "background")
            {
                auto& transform = registry.get<engine::ecs::TransformComponent>(entity);
                transform.position = {center_x, skyCenterY, 0.f};
                transform.scale = {windowWidth, skyHeight, 1.f};
            }
        }
    }

    void LevelPlayState::loadLevel()
    {
        auto& registry = game_.getRegistry();
        const auto physicsWorld = game_.getPhysicsWorld();
        current_level_ = engine::levelLoading::LevelManager::loadLevelByID(level_index_);

        for (auto& object : current_level_->backgrounds)
        {
            engine::ecs::EntityFactory::createDefaultEntity(
                object, registry, physicsWorld);
        }

        for (auto& objGroup : current_level_->groups)
        {
            //compute AABB if it is still on standard values
            if (objGroup.colliderAABB.scale.x <= 1.f || objGroup.colliderAABB.scale.y <= 1.f)
            {
                objGroup.colliderAABB = engine::physics::PhysicsSystem::computeGroupAABB(objGroup.children);
                objGroup.colliderAABB.tag = "platform";
            }
            objGroup.colliderAABB.generateRenderComp = false;
            entt::entity groupAABBEntity = engine::ecs::EntityFactory::createDefaultEntity(
                objGroup.colliderAABB, registry, physicsWorld);

            for (auto& obj : objGroup.children)
            {
                obj.generatePhysicsComp = false;
                glm::vec2 localOffset = {
                    obj.position.x - objGroup.colliderAABB.position.x, obj.position.y - objGroup.colliderAABB.position.y
                };
                const entt::entity entity = engine::ecs::EntityFactory::createDefaultEntity(
                    obj, registry, physicsWorld);
                registry.emplace<engine::ecs::Parent>(entity, &groupAABBEntity, localOffset);
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
        game_.getAudioSystem()->playOneShot("crash");
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
    void LevelPlayState::reloadLevel()
    {
        std::cout << "reload";
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

        for (auto& entity : view)
        {
            auto tag = view.get<engine::ecs::TagComponent>(entity).tag;
            if (!registry.valid(entity) || tag == "ground" || tag == "background") continue;
            engine::ecs::EntityFactory::setPosition(registry, entity,
                                                    registry.get<engine::ecs::TransformComponent>(entity).
                                                             initialPosition);
            engine::ecs::EntityFactory::setScale(registry, entity,
                                                 registry.get<engine::ecs::TransformComponent>(entity).
                                                          initialScale);
            engine::ecs::EntityFactory::SetRotation(registry, entity,
                                                    registry.get<engine::ecs::TransformComponent>(entity).
                                                             initialZRotation);
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

                game_.getAudioSystem()->playOneShot("win");

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
                game_.getContext().setCameraPosAndCenter({0.0f, 0.0f, 1.0f}, {0.f, 0.f, 0.f});

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
