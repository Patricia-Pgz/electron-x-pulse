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
#include "glm/gtc/epsilon.hpp"
#include "ui/UIEvents.h"

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
        const auto bgConfig = getBackgroundSizes(windowBounds);
        auto& registry = game.getRegistry();

        for (const auto entities = registry.view<engine::ecs::TransformComponent, engine::ecs::TagComponent>(); auto
             entity : entities)
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
            else if (tag == "background" || tag == "sky")
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
    LevelBackgroundConfig LevelPlayState::getBackgroundSizes(const std::vector<float>& windowBounds) const
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

    /**create sky entity with color gradient.
     */
    void LevelPlayState::createSkyGradientEntity(const LevelBackgroundConfig& bgConfig, entt::registry& registry,
                                                 const b2WorldId physicsWorld) const
    {
        //no need to make a gradient if both colors are the same
        if (all(epsilonEqual(current_level->gradientBottomColor, current_level->gradientTopColor, 0.001f))) return;
        GameObject sky = {};
        sky.tag = "sky";
        sky.generatePhysicsComp = false;
        sky.vertexShaderPath = "shaders/gradient.vert";
        sky.fragmentShaderPath = "shaders/gradient.frag";
        sky.gradientBottomColor = current_level->gradientBottomColor;
        sky.gradientTopColor = current_level->gradientTopColor;
        sky.position = {bgConfig.centerX, bgConfig.skyCenterY, 0.f};
        sky.scale = {bgConfig.windowWidth, bgConfig.skyHeight, 0.f};
        engine::ecs::EntityFactory::createDefaultEntity(sky, registry, physicsWorld);
    }

    /**
     * Creates background entities for level.
     * @param bgConfig The config with the background sizes
     * @param registry The current enTT registry
     * @param physicsWorld The current Box2D physics world
     */
    void LevelPlayState::createBackgroundEntities(const LevelBackgroundConfig& bgConfig, entt::registry& registry,
                                                  const b2WorldId physicsWorld) const
    {
        for (auto& object : current_level->backgrounds)
        {
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
    }

    /**
     * Creates grouped entities and their physics parent.
     * @param registry The current enTT registry
     * @param physicsWorld The current Box2D physics world
     */
    void LevelPlayState::createGroupedEntities(entt::registry& registry, const b2WorldId physicsWorld) const
    {
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
    }

    /**
     * Creates all single entities.
     * @param registry The current enTT registry
     * @param physicsWorld The current Box2D physics world
     */
    void LevelPlayState::createSingleEntities(entt::registry& registry, const b2WorldId physicsWorld)
    {
        for (auto& object : current_level->objects)
        {
            const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                object, registry, physicsWorld);
            if (object.tag == "player") current_player = entity;
            game.setPlayer(current_player);
        }
    }

    void LevelPlayState::createEntities(const LevelBackgroundConfig& bgConfig, entt::registry& registry,
                                        const b2WorldId physicsWorld)
    {
        createSkyGradientEntity(bgConfig, registry, physicsWorld);
        createBackgroundEntities(bgConfig, registry, physicsWorld);
        createGroupedEntities(registry, physicsWorld);
        createSingleEntities(registry, physicsWorld);
    }

    /**
     * Initializes current level audio.
     */
    void LevelPlayState::initializeAudio()
    {
        game.getAudioSystem()->initializeCurrentAudio(current_level->audioFileName, current_level->playerStartPosX);
        audio_config = game.getAudioSystem()->getConfig();
        //Ensures, that every unit is synced to the beat
        current_level->currentLevelSpeed = current_level->velocityMultiplier / audio_config->seconds_per_beat;
        current_level->levelLength = audio_config->current_audio_length * current_level->currentLevelSpeed;
        //TODO visualisieren!(Editor)
        current_level->finalBeatIndex = audio_config->current_audio_length / audio_config->seconds_per_beat;
        engine::ecs::EventDispatcher::dispatcher.trigger(engine::ecs::LevelLengthComputed{
            current_level->levelLength, current_level->currentLevelSpeed, current_level->finalBeatIndex
        });
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
        const auto bgConfig = getBackgroundSizes(game.getContext().getWorldWindowBounds());
        createEntities(bgConfig, registry, physicsWorld);
        initializeAudio();

        game.getContext().setClearColor(current_level->clearColor);

        level_instantiated = true;
        instruction_ui->setEditMode(edit_mode);

        if (!edit_mode)
        {
            //start level directly if not in edit mode
            game.getAudioSystem()->playCurrentAudio();
            pauseOrResumeLevel(false);
            return;
        }
        //is in edit mode -> deactivate physics and player input
        setSystemsActive(false);
        dynamic_cast<Game&>(game).setPaused(true);
    }

    /**
     * Starts/stops movement of objects towards the player.
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
    void LevelPlayState::pauseOrResumeLevel(const bool pause)
    {
        paused = pause;
        dynamic_cast<Game&>(game).setPaused(pause);
        setSystemsActive(!pause);
        moveObjects(!pause);
        audio_config->audio.setPause(audio_config->currentAudioHandle, pause);
    }

    /**
     * De-/activates physics and player input
     * @param setActive true = activates systems, false deactivates them
     */
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
        pauseOrResumeLevel(event.pauseLevel);
    }

    /**
     * Reacts to PlayerDeath event.
     * @param event Restarts the level on player death event, plays a crash sound.
     */
    void LevelPlayState::onPlayerDeath(const engine::ecs::PlayerDeath& event)
    {
        if(reloading_level) return;
        game.getAudioSystem()->playOneShot("crash");
        onRestartLevel(engine::ui::RestartLevelEvent{true});
    }

    /**
     * Resets all entities to their initial state and starts the level if event.startLevel == true
     * @param event Determines if the game should start after resetting all entities
     */
    void LevelPlayState::onRestartLevel(const engine::ui::RestartLevelEvent& event)
    {
        if(reloading_level) return;
        //game will be reset and stopped if player restarts level and then presses enter in edit mode
        reloadLevel();
        if (!event.startLevel) return;
        startLevel();
    }

    /**
     * Reset all entities to their initial state.
     */
    void LevelPlayState::resetEntities() const
    {
        for (auto& registry = game.getRegistry(); auto& entity : registry.view<entt::entity>())
        {
            if (!registry.valid(entity)) continue;

            //Some entities don't have a RenderComponent, update the ones that do
            if (registry.any_of<engine::ecs::RenderComponent>(entity))
            {
                registry.get<engine::ecs::RenderComponent>(entity).uvOffset = {0.f, 0.f};
            }

            auto& tag = registry.get<engine::ecs::TagComponent>(entity).tag;
            if(tag == "background" || tag == "sky" || tag == "ground") return;

            //reset all transforms to initial state
            auto& transform = registry.get<engine::ecs::TransformComponent>(entity);
            transform.position = transform.initialPosition;
            transform.scale = transform.initialScale;
            transform.zRotation = transform.initialZRotation;

            //Some entities don't have a Physics Component
            if (registry.any_of<engine::ecs::PhysicsComponent>(entity))
            {
                engine::ecs::EntityFactory::setPosition(registry, entity, transform.initialPosition);
                engine::ecs::EntityFactory::setScale(registry, entity, transform.initialScale);
                engine::ecs::EntityFactory::SetRotation(registry, entity, transform.initialZRotation);
            }
        }
    }


    /**
     * Freshly starts the (already reset) level and audio.
     */
    void LevelPlayState::startLevel()
    {
        game.getAudioSystem()->playCurrentAudio();
        pauseOrResumeLevel(false);
    }

    /**
     *Resets every entity to its initial Transform, resets audio, deactivates systems
*/
    void LevelPlayState::reloadLevel()
    {
        if (level_time <= 0.f) return;
        reloading_level = true;
        paused = true;
        dynamic_cast<Game&>(game).setPaused(true);
        setSystemsActive(false);
        menu_ui->setActive(true);
        instruction_ui->setActive(level_index == 0);
        finish_ui->setActive(false);
        game.getAudioSystem()->stopCurrentAudio();
        level_time = 0.f;
        timer = 1.f;
        transition_triggered = false;
        timer_active = false;
        resetEntities();
        reloading_level = false;
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

        if (!timer_active && currentAudioTime >= audio_config->current_audio_length - 0.1f) //slight margin
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
                engine::ecs::EventDispatcher::dispatcher.trigger(events::ShowFinishScreen{true});

                game.getAudioSystem()->playOneShot("win");

                pauseOrResumeLevel(true);
            }
        }
    }

    /**
     * Resets everything on returning to the level selection screen.
     * Clears the game's entity registry.
     */
    void LevelPlayState::unloadLevel()
    {
        level_time = 0.f;
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
            level_time += deltaTime;
            delayLevelEnd(deltaTime);
        }
    }
}
