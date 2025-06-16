#include "GameStateManager.h"
#include <random>

#include "InGameMenuSystem.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/levelloading/LevelLoader.h"
#include "state/LevelSelectState.h"
#include "engine/stateManagement/StateManagerSystem.h"
#include "engine/userInterface/UISystem.h"
#include "State/LevelPlayState.h"

namespace gl3::game
{
    GameStateManager::GameStateManager(Game& game) : game_(game)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().connect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().connect<&
            GameStateManager::onPlayerDeath>(this);
        onUIInitHandle = game_.getUISystem().onInitialized.addListener([this]()
        {
            onUiInitialized();
        });
    }

    GameStateManager::~GameStateManager()
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().disconnect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().disconnect<&
            GameStateManager::onPlayerDeath>(this);
        game_.getUISystem().onInitialized.removeListener(onUIInitHandle);
    }

    void GameStateManager::onUiInitialized() const
    {
        auto& levelUI = dynamic_cast<engine::levelLoading::LevelSelectUISystem&>(game_.getUISystem().getSubsystems(0));
        game_.getStateManagement().pushState<engine::state::LevelSelectState>(levelUI);
    }


    /*std::vector<GameObject> generateTestObjects(const float& initialPlayerPositionX)
    {
        std::cout << "generate objects";
        std::mt19937 randomNumberEngine{
            static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
        };
        std::uniform_real_distribution yScaleDist{0.3, 0.3};
        std::uniform_real_distribution xScaleDist{0.4, 0.7};
        std::uniform_real_distribution blueColorDist{0.3, 1.0};

        std::vector<glm::vec4> blueColors;
        std::vector<glm::vec4> purpleColors;

        /*auto c = blueColorDist(randomNumberEngine);
        auto randomBlueColor = glm::vec4(0.1, c, c, 1.0f);
        glm::vec4 purpleColor{1.f,0.f,1.f,1.f};#1#
        glm::vec4 redColor{1.f, 0.f, 0.f, 1.f};

        size_t index = 0;

        double previousC = 0.0;

        for (int i = 0; i <= 5; i++)
        {
            auto c = blueColorDist(randomNumberEngine);
            while (c == previousC) { c = blueColorDist(randomNumberEngine); }
            blueColors.push_back(glm::vec4(0.16f, 0.1f, c, 1.0f));
            purpleColors.push_back(glm::vec4(c, 0.f, 0.2, 1.0f));
            previousC = c;
        }
        std::vector<GameObject> game_objects = {
            {4 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[0]},
            {5 + initialPlayerPositionX, 0.f, false, 0.5f, 0.5f, redColor},

            {11 + initialPlayerPositionX, 0.f, true, 1, 2, blueColors[1]},

            {
                16 + initialPlayerPositionX, 0.f, true, 1, 1,
                purpleColors[0]
            },
            {
                18 + initialPlayerPositionX, 0.f, true, 2, 1,
                purpleColors[1]
            },
            {
                20 + initialPlayerPositionX, 0.f, true, 3, 1,
                purpleColors[0]
            },
            {
                22 + initialPlayerPositionX, 0.f, true, 4, 1,
                purpleColors[1]
            },

            {28 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[0]},
            {
                28 + initialPlayerPositionX + 0.75f, 0.f, false, 0.5f,
                0.5f, redColor
            },
            {
                28 + initialPlayerPositionX + 1.25f, 0.f, false, 0.5f,
                0.5f, redColor
            },
            {30 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[0]},
            {32 + initialPlayerPositionX, 0.f, true, 2, 1, blueColors[1]},

            {
                38 + initialPlayerPositionX, 0.f, true, 1, 1,
                purpleColors[1]
            },
            {
                39 + initialPlayerPositionX, 0.f, false, 0.5f, 0.5f,
                redColor
            },
            {
                40 + initialPlayerPositionX, 0.f, true, 1, 1,
                purpleColors[3]
            },
            {
                41 + initialPlayerPositionX, 0.f, false, 0.5f, 0.5f,
                redColor
            },
            {
                42 + initialPlayerPositionX, 0.f, true, 2, 1,
                purpleColors[1]
            },
            {
                43 + initialPlayerPositionX, 0.f, false, 0.5f, 0.5f,
                redColor
            },

            {47 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[0]},

            {50 + initialPlayerPositionX, 0.f, true, 2, 1, blueColors[1]},
            {52 + initialPlayerPositionX, 0.f, true, 2, 1, blueColors[1]},
            {
                54 + initialPlayerPositionX, 0.f, true, 1, 2,
                purpleColors[2]
            },
            {56 + initialPlayerPositionX, 0.f, true, 2, 1, blueColors[4]},
            {
                58 + initialPlayerPositionX, 0.f, true, 1, 1,
                purpleColors[1]
            },
            {60 + initialPlayerPositionX, 0.f, true, 2, 1, blueColors[4]},
            {
                62 + initialPlayerPositionX, 0.f, true, 3, 1,
                purpleColors[3]
            },
            {64 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[1]},
            {
                66 + initialPlayerPositionX, 0.f, true, 1, 1,
                purpleColors[3]
            },
            {
                68 + initialPlayerPositionX, 0.f, true, 2, 1,
                purpleColors[4]
            },
            {
                70 + initialPlayerPositionX, 0.f, true, 1, 1,
                purpleColors[3]
            },
            {
                72 + initialPlayerPositionX, 0.f, true, 1, 1,
                purpleColors[4]
            },
            {
                74 + initialPlayerPositionX, 0.f, true, 2, 1,
                purpleColors[1]
            },
            {
                76 + initialPlayerPositionX, 0.f, true, 3, 1,
                purpleColors[0]
            },
            {
                78 + initialPlayerPositionX, 0.f, true, 4, 1,
                purpleColors[1]
            },

            {80 + initialPlayerPositionX, 3.0f, true, 1, 3, blueColors[0]},
            {
                80 + initialPlayerPositionX + 0.25f, 3.0f, false,
                0.5f, 0.5f, redColor
            },
            {
                84 + initialPlayerPositionX, 4.0f, true, 1, 2,
                purpleColors[3]
            },
            {
                84.f + initialPlayerPositionX + 0.75f, 4.0f, false,
                0.5f, 0.5f, redColor
            },

            {86 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[0]},

            {91 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[1]},
            {93 + initialPlayerPositionX, 0.f, true, 2, 1, blueColors[1]},

            {98 + initialPlayerPositionX, 0.f, true, 1, 1, blueColors[0]},
            {
                100 + initialPlayerPositionX, 0.f, true, 2, 1,
                purpleColors[0]
            },
            {102 + initialPlayerPositionX, 0.f, true, 3, 1, blueColors[3]},
            {104 + initialPlayerPositionX, 0.f, true, 4, 1, blueColors[3]},
            {106 + initialPlayerPositionX, 0.f, true, 5, 1, blueColors[1]},
            {
                108 + initialPlayerPositionX, 0.f, true, 2, 1,
                purpleColors[0]
            },
            {110 + initialPlayerPositionX, 0.f, true, 3, 1, blueColors[1]},
            {112 + initialPlayerPositionX, 0.f, true, 4, 1, blueColors[2]},
            {114 + initialPlayerPositionX, 0.f, true, 5, 1, blueColors[1]},
        };

        return game_objects;
    }*/

    void GameStateManager::onGameStateChange(const engine::ecs::GameStateChange& event)
    {
        auto& stateSystem = game_.getStateManagement();
        auto& gameConfig = game_.getCurrentConfig();

        if (event.newGameState == engine::GameState::LevelSelect)
        {
            auto& levelUI = dynamic_cast<engine::levelLoading::LevelSelectUISystem&>(game_.getUISystem().
                getSubsystems(0));
            stateSystem.changeState<engine::state::LevelSelectState>(levelUI);
            return;
        }

        if (event.newGameState == engine::GameState::Level && previous_game_state_ != engine::GameState::Level)
        {
            auto& menuUI = dynamic_cast<ui::InGameMenuSystem&>(game_.getUISystem().
                                                                     getSubsystems(1));
            stateSystem.changeState<state::LevelPlayState>(menuUI, event.newLevelIndex, game_);
            return;
        }

        return;
        const auto& config = game_.getCurrentConfig();

        // initial_test_game_objects = generateTestObjects(config.initial_player_position_x);

        if (current_game_state_ == engine::GameState::Level || current_game_state_ ==
            engine::GameState::PreviewWithTesting ||
            current_game_state_ == engine::GameState::PreviewWithScrolling)
        {
            /*for (auto& object : initial_test_game_objects)
            {
                auto posY = object.positionY == 0
                                ? config.ground_level + object.scaleY / 2
                                : config.ground_level - object.scaleY / 2 + object.positionY;

                if (object.isPlatform)
                {
                    const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                        game_.getRegistry(), glm::vec3(object.positionX, posY, 0.0f),
                        object.color, "platform", game_.getPhysicsWorld());
                    engine::ecs::EntityFactory::setScale(game_.getRegistry(), entity,
                                                         glm::vec3(object.scaleX, object.scaleY, 0.f));
                    object.entityID = entity;
                    object.positionY = posY;
                }
                else
                {
                    const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                        game_.getRegistry(), glm::vec3(object.positionX, posY, 0.0f),
                        object.color, "obstacle", game_.getPhysicsWorld(), true);
                    engine::ecs::EntityFactory::setScale(game_.getRegistry(), entity,
                                                         glm::vec3(object.scaleX, object.scaleY, 0.f));
                    object.entityID = entity;
                    object.positionY = posY;
                }
            }*/

            /*int index = 0;
            for (auto beat : beatPositions)
            {
                if (index < 1)
                {
                    index++;
                    continue;
                }

                if (index % 2 == 0)
                {
                    auto height = 0.25f;
                    auto randomColor = glm::vec4(0.1, 0.5, 0.3, 1.0f);
                    engine::ecs::EntityFactory::createDefaultEntity(
                        registry_, glm::vec3(beat, groundLevel + height / 2, 0.0f), glm::vec3(0.5f, height, 0.f),
                        0.f, randomColor, "platform", physicsWorld);
                }
                else
                {
                    engine::ecs::EntityFactory::createDefaultEntity(registry_, glm::vec3(beat, -0.875f, 0.0f),
                                                                    glm::vec3(0.5f, 0.5f, 0.f),
                                                                    0.f, glm::vec4(1.0, 0.1, 0.05, 1), "obstacle",
                                                                    physicsWorld);
                }
                index++;
            }*/
        }

        /*if (current_game_state_ == engine::GameState::PreviewWithScrolling || current_game_state_ ==
            engine::GameState::PreviewWithTesting)
        {
            const auto timeLine = engine::ecs::EntityFactory::createDefaultEntity(
                game_.getRegistry(), glm::vec3(0, config.ground_level, 0.0f), glm::vec4(0.1, 0.1, 1.0, 1.0f),
                "timeline",
                game_.getPhysicsWorld());
            engine::ecs::EntityFactory::setScale(game_.getRegistry(), timeLine, glm::vec3(40.f, 0.05f, 0.f));

            std::vector<GameObject> tempObjects;
            tempObjects.push_back(GameObject(0.f, config.ground_level, true, 40.f, 0.05f,
                                             glm::vec4(0.1, 0.1, 1.0, 1.0f),
                                             timeLine));

            for (auto beatPosition : beatPositions)
            {
                auto timeLineColor = glm::vec4(0.1, 0.1, 1.0, 1.0f);
                const auto beat = engine::ecs::EntityFactory::createDefaultEntity(
                    game_.getRegistry(), glm::vec3(beatPosition, config.ground_level, 0.0f), timeLineColor, "beat",
                    game_.getPhysicsWorld());
                engine::ecs::EntityFactory::setScale(game_.getRegistry(), beat, glm::vec3(0.05f, 0.5f, 0.f));
                tempObjects.push_back(GameObject(beatPosition, config.ground_level, true, 0.05f, 0.5f, timeLineColor,
                                                 beat));
            }
            initial_test_game_objects = tempObjects;
        }*/


        previous_game_state_ = current_game_state_;
        current_game_state_ = event.newGameState;
    }

    void GameStateManager::resetComponents()
    {
        /*std::cout << "reset comps";
        const auto view = game_.getRegistry().view<engine::ecs::TransformComponent, engine::ecs::TagComponent,
                                                   engine::ecs::PhysicsComponent>();
        for (auto entity : view)

        {
            auto& transform = view.get<engine::ecs::TransformComponent>(entity);
            auto& tag = view.get<engine::ecs::TagComponent>(entity).tag;
            auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            b2Body_SetLinearVelocity(physics_comp.body, {0.f, 0.0f});

            if (tag == "player")
            {
                engine::ecs::EntityFactory::setPosition(game_.getRegistry(), entity,
                                                        glm::vec3(game_.getCurrentConfig().initial_player_position_x,
                                                                  game_.getCurrentConfig().ground_level + transform.
                                                                  scale.y / 2, 0.f));
                continue;
            }
            if (tag == "ground")
            {
                engine::ecs::EntityFactory::setPosition(game_.getRegistry(), entity,
                                                        glm::vec3(
                                                            0, game_.getCurrentConfig().ground_level - game_.
                                                            getCurrentConfig().ground_height / 2, 0.0f));
                continue;
            }
            b2Body_SetLinearVelocity(physics_comp.body, {game_.getCurrentConfig().level_speed(), 0.f});
            auto obj = std::ranges::find_if(initial_test_game_objects,
                                            [entity](const GameObject& o)
                                            {
                                                return o.entityID == entity;
                                            });
            engine::ecs::EntityFactory::setPosition(game_.getRegistry(), entity,
                                                    glm::vec3(obj->positionX, obj->positionY, 0.f));
        }*/
    }

    void GameStateManager::reset()
    {
        std::cout << "reset";

        //TODO resetCurrentAudio + timing verbessern


        // Reset all entities to their initial states
        resetComponents();
        if (current_game_state_ != engine::GameState::Menu && current_game_state_ !=
            engine::GameState::PreviewWithScrolling)
        {
            const auto& entities = game_.getRegistry().view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();

            for (auto& entity : entities)
            {
                auto& physics_comp = entities.get<engine::ecs::PhysicsComponent>(entity);
                auto& tag_comp = entities.get<engine::ecs::TagComponent>(entity);
                if (tag_comp.tag == "platform" || tag_comp.tag == "obstacle")
                {
                    //b2Body_SetLinearVelocity(physics_comp.body, {game_.getCurrentConfig().level_speed(), 0.0f});
                }
            }
        }
        is_resetting_ = false;
    }

    void GameStateManager::onPlayerDeath(engine::ecs::PlayerDeath& event)
    {
        std::cout << "Player died! Calling reset..." << std::endl;
        is_resetting_ = true;
        reset();
    }
} // gl3
