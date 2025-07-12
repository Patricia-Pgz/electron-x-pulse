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
        if (!level_instantiated_)return;
        const auto windowBounds = *event.windowBounds;
        const auto bgConfig = updateBackgrounds(windowBounds);
        auto& registry = game_.getRegistry();
        const auto entities = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent>();

        for (auto entity : entities)
        {
            if (auto tag = entities.get<engine::ecs::TagComponent>(entity).tag; tag == "ground")
            {
                engine::ecs::EntityFactory::setPosition(registry, entity, {
                                                            bgConfig.centerX, bgConfig.groundCenterY, 0.f
                                                        });
                engine::ecs::EntityFactory::setScale(registry, entity, {
                                                         bgConfig.windowWidth, bgConfig.groundHeight, 1.f
                                                     });
            }
            else if (tag == "background")
            {
                auto& transform = registry.get<engine::ecs::TransformComponent>(entity);
                transform.position = {bgConfig.centerX, bgConfig.skyCenterY, 0.f};
                transform.scale = {bgConfig.windowWidth, bgConfig.skyHeight, 1.f};
            }
        }
    }

    /**
     *@short Calculates the scales and positions for the two horizontally divided background visuals.
*The Background is divided horizontally by the groundlevel. In the upper portion, objects tagged "background" get layered on top of each other,
*in the order in which they appear in the level's .json file @see LevelPlayState::loadLevel()
*Same for the bottom part, which layers objects tagged as "ground".
*@warning This does not keep aspect ratios of textures, it just fits the object sizes and positions to the screen. May stretch textures!
*/
    LevelBackgroundConfig LevelPlayState::updateBackgrounds(const std::vector<float>& windowBounds) const
    {
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

        return {center_x, windowWidth, groundCenterY, groundHeight, skyCenterY, skyHeight};
    }

    /**
     * Loads the selected level via LevelManager, instantiates all entities from the loaded data structure (GameObject/Level),
     * calls on AudioSystem to instantiate the current config = analyse audio, creates AABB for grouped objects,
     * calculates levelLength, speed and final beat, and sends the current player to Game.
     * Then sets up ui and internal properties for starting the level/edit mode.
     */
    void LevelPlayState::loadLevel()
    {
        auto& registry = game_.getRegistry();
        const auto physicsWorld = game_.getPhysicsWorld();
        current_level_ = engine::levelLoading::LevelManager::loadLevelByID(level_index_);

        for (auto& object : current_level_->backgrounds)
        {
            const auto bgConfig = updateBackgrounds(game_.getContext().getWorldWindowBounds());
            if (object.tag == "ground")
            {
                object.position = {bgConfig.centerX, bgConfig.groundCenterY, 0.f};
                object.scale = {bgConfig.windowWidth, bgConfig.groundHeight, 0.f};
            }
            else
            {
                object.position = {bgConfig.centerX, bgConfig.skyCenterY, 0.f};
                object.scale = {bgConfig.windowWidth, bgConfig.skyHeight, 0.f};
            }
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
        //TODO visualisieren!(Editor)
        current_level_->finalBeatIndex = audio_config_->current_audio_length / audio_config_->seconds_per_beat;
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::LevelLengthComputed{
            current_level_->levelLength, current_level_->currentLevelSpeed, current_level_->finalBeatIndex
        });

        game_.getContext().setClearColor(current_level_->clearColor);

        level_instantiated_ = true;
        instruction_ui_->setEditMode(edit_mode_);
        instruction_ui_->pauseTimer(edit_mode_); //pause timer if in edit mode

        if (!edit_mode_)
        {
            //start level directly if not in edit mode
            game_.getAudioSystem()->playCurrentAudio();
            pauseOrStartLevel(false);
            return;
        }
        //is in edit mode -> deactivate physics and player input
        setSystemsActive(false);
    }

    /**
     * Starts movement of objects towards the player.
     * @param move determines if the objects should start or stop moving towards the player.
     */
    void LevelPlayState::moveObjects(const bool move) const
    {
        for (const auto view = game_.getRegistry().view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();
             auto& entity : view)
        {
            if (!game_.getRegistry().valid(entity) || entity == entt::null)return;
            const auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            if (auto& tag = view.get<engine::ecs::TagComponent>(entity).tag; tag == "platform" || tag == "obstacle")
            {
                if (move)
                {
                    b2Body_SetLinearVelocity(physics_comp.body, {current_level_->currentLevelSpeed * -1, 0.0f});
                }
                else
                {
                    b2Body_SetLinearVelocity(physics_comp.body, {0.f, 0.0f});
                }
            }
        }
    }


    /**
     * Pauses or resumes the level. @note This does not reset entities, audio, etc. it just stops/resumes audio, movement, and timers.
     * @param pause Bool that determines if the level should be paused or resumed.
     */
    void LevelPlayState::pauseOrStartLevel(const bool pause)
    {
        paused = pause;
        setSystemsActive(!pause);
        moveObjects(!pause);
        audio_config_->audio.setPause(audio_config_->currentAudioHandle, pause);
        instruction_ui_->pauseTimer(pause);
    }

    void LevelPlayState::setSystemsActive(const bool setActive) const
    {
        game_.getPhysicsSystem()->setActive(setActive);
        dynamic_cast<Game&>(game_).getPlayerInputSystem()->setActive(setActive);
    }

    /**
     * Pauses or resumes level when engine::ui::PauseLevelEvent was triggered.
     * @param event PauseLevelEvent, is sent by UI and determines to pause or resume the level.
     */
    void LevelPlayState::onPauseEvent(const engine::ui::PauseLevelEvent& event)
    {
        pauseOrStartLevel(event.pauseLevel);
    }

    /**
     *
     * @param event Resarts the level on player death event, plays a crash sound.
     */
    void LevelPlayState::onPlayerDeath(const engine::ecs::PlayerDeath& event)
    {
        game_.getAudioSystem()->playOneShot("crash");
        onRestartLevel(engine::ui::RestartLevelEvent{true});
    }

    /**
     * Resets all entities to their initial state and starts the level if
     * @param event Determines if the game should start after resetting all entities
     */
    void LevelPlayState::onRestartLevel(const engine::ui::RestartLevelEvent& event)
    {
        //game will be reset and stopped if player restarts level and then presses enter in edit mode
        reloadLevel();
        if (!event.startLevel) return;
        startLevel();
    }

    /**
     * Freshly starts the (already reset) level and audio.
     */
    void LevelPlayState::startLevel()
    {
        game_.getAudioSystem()->playCurrentAudio();
        pauseOrStartLevel(false);
    }

    /**
     *Resets every entity to its initial Transform, resets audio, deactivates systems
*/
    void LevelPlayState::reloadLevel()
    {
        std::cout << "reload";
        setSystemsActive(false);
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

    /**
     * Starts a timer, when the end of the audio track is reached -> level is won.
     * Shows a winning screen and sound after expiration of the timer.
     * @param deltaTime The game's time since the previous frame.
     */
    void LevelPlayState::delayLevelEnd(const float deltaTime)
    {
        const auto currentAudioTime = static_cast<float>(audio_config_->audio.getStreamTime(
            audio_config_->currentAudioHandle));

        if (!timer_active_ && currentAudioTime >= audio_config_->current_audio_length - 1) //slight margin
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

    /**
     * Resets everything on returning to the level selection screen.
     * Clears the game's entity registry.
     */
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
    }

    void LevelPlayState::update(const float deltaTime)
    {
        if (!level_instantiated_)
        {
            return;
        }
        if (!paused)
        {
            engine::visual_effects::Parallax::moveBgObjectsParallax( //TODO über UVs
                game_.getRegistry(), deltaTime, current_level_->currentLevelSpeed);
            delayLevelEnd(deltaTime);
        }
    }
}
