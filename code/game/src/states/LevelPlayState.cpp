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
        if (!level_instantiated)return;
        const auto windowBounds = *event.windowBounds;
        const auto bgConfig = updateBackgrounds(windowBounds);
        auto& registry = game.getRegistry();
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

        const float groundLevel = current_level->groundLevel;

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
        auto& registry = game.getRegistry();
        const auto physicsWorld = game.getPhysicsWorld();
        current_level = engine::levelLoading::LevelManager::loadLevelByID(level_index);

        for (auto& object : current_level->backgrounds)
        {
            const auto bgConfig = updateBackgrounds(game.getContext().getWorldWindowBounds());
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


        auto& objGroup = current_level->groups;
        for (auto group = objGroup.begin(); group != objGroup.end();)
        {
            if (group->children.empty())
            {
                group = objGroup.erase(group); // erase empty groups
            }
            else
            {
                //compute AABB if it is still on standard values
                if (group->colliderAABB.scale.x <= 1.f || group->colliderAABB.scale.y <= 1.f)
                {
                    group->colliderAABB = engine::physics::PhysicsSystem::computeGroupAABB(group->children);
                    group->colliderAABB.tag = "platform";
                }
                group->colliderAABB.generateRenderComp = false;
                entt::entity groupAABBEntity = engine::ecs::EntityFactory::createDefaultEntity(
                    group->colliderAABB, registry, physicsWorld);
                std::vector<entt::entity> childEntities;

                for (auto& obj : group->children)
                {
                    obj.generatePhysicsComp = false;
                    glm::vec2 localOffset = {
                        obj.position.x - group->colliderAABB.position.x, obj.position.y - group->colliderAABB.position.y
                    };
                    const entt::entity entity = engine::ecs::EntityFactory::createDefaultEntity(
                        obj, registry, physicsWorld);
                    registry.emplace<engine::ecs::ParentComponent>(entity, groupAABBEntity, localOffset);
                    childEntities.push_back(entity);
                }

                registry.emplace<engine::ecs::GroupComponent>(groupAABBEntity, childEntities);

                ++group;
            }
        }

        float initialPlayerPosX = 0.f;
        for (auto& object : current_level->objects)
        {
            const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                object, registry, physicsWorld);
            if (object.tag == "player") current_player = entity;
            initialPlayerPosX = object.position.x;
            game.setPlayer(current_player);
        }
        game.getAudioSystem()->initializeCurrentAudio(current_level->audioFile, initialPlayerPosX);
        audio_config = game.getAudioSystem()->getConfig();
        //Ensures, that every unit is synced to the beat
        current_level->currentLevelSpeed = current_level->velocityMultiplier / audio_config->seconds_per_beat;
        current_level->levelLength = audio_config->current_audio_length * current_level->currentLevelSpeed;
        //TODO visualisieren!(Editor)
        current_level->finalBeatIndex = audio_config->current_audio_length / audio_config->seconds_per_beat;
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::LevelLengthComputed{
            current_level->levelLength, current_level->currentLevelSpeed, current_level->finalBeatIndex
        });

        game.getContext().setClearColor(current_level->clearColor);

        level_instantiated = true;
        menu_ui->setEditMode(edit_mode);
        instruction_ui->setEditMode(edit_mode);
        instruction_ui->pauseTimer(edit_mode); //pause timer if in edit mode

        if (!edit_mode)
        {
            //start level directly if not in edit mode
            game.getAudioSystem()->playCurrentAudio();
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
        for (const auto view = game.getRegistry().view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();
             auto& entity : view)
        {
            if (!game.getRegistry().valid(entity) || entity == entt::null)return;
            const auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            if (auto& tag = view.get<engine::ecs::TagComponent>(entity).tag; tag == "platform" || tag == "obstacle")
            {
                if (move)
                {
                    b2Body_SetLinearVelocity(physics_comp.body, {current_level->currentLevelSpeed * -1, 0.0f});
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
        dynamic_cast<Game&>(game).setPaused(pause);
        setSystemsActive(!pause);
        moveObjects(!pause);
        audio_config->audio.setPause(audio_config->currentAudioHandle, pause);
        instruction_ui->pauseTimer(pause);
    }

    void LevelPlayState::setSystemsActive(const bool setActive) const
    {
        game.getPhysicsSystem()->setActive(setActive);
        dynamic_cast<Game&>(game).getPlayerInputSystem()->setActive(setActive);
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
        game.getAudioSystem()->playOneShot("crash");
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
        if (!event.startLevel || edit_mode) return;
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::EditorPlayModeChange{true});
        startLevel();
    }

    void LevelPlayState::onPhysicsStepDone()
    {
        if (!reset_level) return;
        reset_level = false;
        auto& registry = game.getRegistry();
        const auto singleEntities = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent,
                                                  engine::ecs::PhysicsComponent, engine::ecs::RenderComponent>();

        //go through ungrouped entities, reset and activate components
        for (auto& entity : singleEntities)
        {
            if (auto tag = singleEntities.get<engine::ecs::TagComponent>(entity).tag; !registry.valid(entity) || tag ==
                "ground"
                || tag == "background")
                continue;
            engine::ecs::EntityFactory::setPosition(registry, entity,
                                                    singleEntities.get<engine::ecs::TransformComponent>(entity).
                                                                   initialPosition);
            engine::ecs::EntityFactory::setScale(registry, entity,
                                                 singleEntities.get<engine::ecs::TransformComponent>(entity).
                                                                initialScale);
            engine::ecs::EntityFactory::SetRotation(registry, entity,
                                                    singleEntities.get<engine::ecs::TransformComponent>(entity).
                                                                   initialZRotation);
            singleEntities.get<engine::ecs::PhysicsComponent>(entity).isActive = true;
            singleEntities.get<engine::ecs::RenderComponent>(entity).isActive = true;
        }

        //go through group parent entities, reset and activate, and later same for group child entities

        for (const auto groupParents = registry.view<
                 engine::ecs::GroupComponent, engine::ecs::TransformComponent, engine::ecs::PhysicsComponent>(); auto
             & entity : groupParents)
        {
            engine::ecs::EntityFactory::setPosition(registry, entity,
                                                    groupParents.get<engine::ecs::TransformComponent>(entity).
                                                                 initialPosition);
            engine::ecs::EntityFactory::setScale(registry, entity,
                                                 groupParents.get<engine::ecs::TransformComponent>(entity).
                                                              initialScale);
            engine::ecs::EntityFactory::SetRotation(registry, entity,
                                                    groupParents.get<engine::ecs::TransformComponent>(entity).
                                                                 initialZRotation);
            //only activate physics, has no renderer
            groupParents.get<engine::ecs::PhysicsComponent>(entity).isActive = true;
        }

        const auto groupChildEntities = registry.view<engine::ecs::ParentComponent, engine::ecs::TransformComponent,
                                                      engine::ecs::RenderComponent>();
        for (auto& entity : groupChildEntities)
        {
            if (!registry.valid(entity))continue;
            auto& transform = groupChildEntities.get<engine::ecs::TransformComponent>(entity);
            transform.position = transform.initialPosition;
            transform.scale = transform.initialScale;
            transform.zRotation = transform.initialZRotation;
            //only activate renderer, has no physics -> follows parent
            groupChildEntities.get<engine::ecs::RenderComponent>(entity).isActive = true;
        }
    }


    /**
     * Freshly starts the (already reset) level and audio.
     */
    void LevelPlayState::startLevel()
    {
        game.getAudioSystem()->playCurrentAudio();
        pauseOrStartLevel(false);
    }

    /**
     *Resets every entity to its initial Transform, resets audio, deactivates systems
*/
    void LevelPlayState::reloadLevel()
    {
        if (levelTime <= 0.f) return;
        paused = true;
        dynamic_cast<Game&>(game).setPaused(true);
        reset_level = true;
        setSystemsActive(false);
        menu_ui->setActive(true);
        instruction_ui->setActive(level_index == 0);
        finish_ui->setActive(false);
        game.getAudioSystem()->stopCurrentAudio();
        levelTime = 0.f;
        timer = 2.f;
        transition_triggered = false;
        timer_active = false;
    }

    /**
     * Starts a timer, when the end of the audio track is reached -> level is won.
     * Shows a winning screen and sound after expiration of the timer.
     * @param deltaTime The game's time since the previous frame.
     */
    void LevelPlayState::delayLevelEnd(const float deltaTime)
    {
        const auto currentAudioTime = static_cast<float>(audio_config->audio.getStreamTime(
            audio_config->currentAudioHandle));

        if (!timer_active && currentAudioTime >= audio_config->current_audio_length - 1) //slight margin
        {
            timer_active = true;
        }

        if (timer_active)
        {
            timer -= deltaTime;

            if (timer <= 0.0f && !transition_triggered)
            {
                transition_triggered = true;

                menu_ui->setActive(false);
                instruction_ui->setActive(false);
                finish_ui->setActive(true);

                game.getAudioSystem()->playOneShot("win");

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
        levelTime = 0.f;
        level_instantiated = false;
        game.getAudioSystem()->stopCurrentAudio();
        game.getAudioSystem()->stopAllOneShots();
        menu_ui->setActive(false);
        instruction_ui->setActive(false);
        finish_ui->setActive(false);
        menu_ui = nullptr;
        instruction_ui = nullptr;
        finish_ui = nullptr;

        engine::ecs::EntityFactory::clearRegistry(game.getRegistry());
        level_index = -1;
        current_level = nullptr;
        current_player = entt::null;
    }

    void LevelPlayState::update(const float deltaTime)
    {
        if (!level_instantiated)
        {
            return;
        }
        if (!paused)
        {
            levelTime += deltaTime;
            engine::visual_effects::Parallax::moveBgObjectsParallax( //TODO über UVs
                game.getRegistry(), deltaTime, current_level->currentLevelSpeed);
            delayLevelEnd(deltaTime);
        }
    }
}
