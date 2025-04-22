#include "Game.h"
#include <random>
#include <iostream>
#include "Assets.h"
#include "PlayerInputSystem.h"
#include "physics/PlayerContactListener.h"
#include "engine/AudioAnalysis.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/rendering/RenderingSystem.h"

namespace gl3
{
    Game::Game(const int width, const int height, const std::string& title, const glm::vec3& camPos,
               const float camZoom)
        : engine::Game(width, height, title, camPos, camZoom), physics_system_(*this), rendering_system_(*this),
          player_input_system_(*this)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().connect<&Game::onPlayerDeath>(this);
    }

    void Game::scroll_callback_fun(const double yOffset)
    {
        if (currentGameState != engine::GameState::PreviewWithScrolling) return;
        float cameraX = 0.0f; /*cameraPosition.x;*/
        float scrollSpeed = 0.5f;
        float minScrollX = 0.0f; // Minimum scroll limit
        int width, height;
        glfwGetWindowSize(getWindow(), &width, &height);
        float maxScrollX = (levelLength + static_cast<float>(width) / 2 * context.getCurrentZoom() -
            initialPlayerPositionX);
        // Maximum scroll limit

        cameraX = static_cast<float>(yOffset) * scrollSpeed;
        if (cameraX < minScrollX) cameraX = minScrollX;
        if (cameraX > maxScrollX) cameraX = maxScrollX;

        moveEntitiesScrolling();

        auto newCameraPosition = glm::vec3(cameraX, context.getCameraPos().y, context.getCameraPos().z);
        auto newCameraCenter = glm::vec3(cameraX, context.getCameraCenter().y, context.getCameraCenter().z);
        context.setCameraPos(newCameraPosition);
        context.setCameraCenter(newCameraCenter);
    }


    void Game::moveEntitiesScrolling()
    {
        auto entities = registry_.view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();

        for (auto entity : entities)
        {
            auto& tag_component = entities.get<engine::ecs::TagComponent>(entity);
            auto& physics_component = entities.get<engine::ecs::PhysicsComponent>(entity);
            if (tag_component.tag != "beat" && tag_component.tag != "timeline")
            {
                b2Body_SetTransform(physics_component.body,
                                    b2Vec2(b2Body_GetPosition(physics_component.body).x + context.getCameraPos().x,
                                           b2Body_GetPosition(physics_component.body).y),
                                    b2Body_GetRotation(physics_component.body));
            }
        }
    }

    void Game::start()
    {
        context.onScrolling.addListener([&](const float offsetY)
        {
            scroll_callback_fun(offsetY);
        });

        const auto& ground = engine::ecs::EntityFactory::createDefaultEntity(
            registry_, glm::vec3(0, groundLevel - groundHeight / 2, 0.0f), glm::vec4(0.25, 0.27, 1, 1), "ground",
            physicsWorld);
        engine::ecs::EntityFactory::setScale(registry_, ground, glm::vec3(40.f, groundHeight, 0.f));
        player = engine::ecs::EntityFactory::createDefaultEntity(
            registry_, glm::vec3(initialPlayerPositionX, groundLevel + 0.25 / 2, 0),
            glm::vec4(0.25f, 0.25f, 0.25f, 1.0f), "player", physicsWorld);
        engine::ecs::EntityFactory::setScale(registry_, player, glm::vec3(0.25f, 0.25f, 0.f));
        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(resolveAssetPath("audio/SensesShort.wav").c_str());
        backgroundMusic->setLooping(false);
    }

    void Game::update(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            currentGameState = engine::GameState::Level;
            //TODO Preview funktioniert nicht mehr, prüfen wie ich entities generiere!!
        }

        if (currentGameState != previousGameState)
        {
            previousGameState = currentGameState;
            onGameStateChange();
        }
        player_input_system_.update(player);
    }

    void Game::draw()
    {
        rendering_system_.draw();
    }

    //TODO delete entities when levelReload/Load/wechsel

    void Game::updatePhysics()
    {
        if (currentGameState == engine::GameState::Menu || isResetting) return;
        physics_system_.runPhysicsStep();
    }

    std::vector<GameObject> generateTestObjects(const float& beatInterval, const float& initialPlayerPositionX)
    {
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
        glm::vec4 purpleColor{1.f,0.f,1.f,1.f};*/
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
            {4 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},
            {5 * beatInterval + initialPlayerPositionX, 0.f, false, 0.5f * beatInterval, 0.5f * beatInterval, redColor},

            {11 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 2 * beatInterval, blueColors[1]},

            {
                16 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval,
                purpleColors[0]
            },
            {
                18 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval,
                purpleColors[1]
            },
            {
                20 * beatInterval + initialPlayerPositionX, 0.f, true, 3 * beatInterval, 1 * beatInterval,
                purpleColors[0]
            },
            {
                22 * beatInterval + initialPlayerPositionX, 0.f, true, 4 * beatInterval, 1 * beatInterval,
                purpleColors[1]
            },

            {28 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},
            {
                28 * beatInterval + initialPlayerPositionX + 0.75f * beatInterval, 0.f, false, 0.5f * beatInterval,
                0.5f * beatInterval, redColor
            },
            {
                28 * beatInterval + initialPlayerPositionX + 1.25f * beatInterval, 0.f, false, 0.5f * beatInterval,
                0.5f * beatInterval, redColor
            },
            {30 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},
            {32 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},

            {
                38 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval,
                purpleColors[1]
            },
            {
                39 * beatInterval + initialPlayerPositionX, 0.f, false, 0.5f * beatInterval, 0.5f * beatInterval,
                redColor
            },
            {
                40 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval,
                purpleColors[3]
            },
            {
                41 * beatInterval + initialPlayerPositionX, 0.f, false, 0.5f * beatInterval, 0.5f * beatInterval,
                redColor
            },
            {
                42 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval,
                purpleColors[1]
            },
            {
                43 * beatInterval + initialPlayerPositionX, 0.f, false, 0.5f * beatInterval, 0.5f * beatInterval,
                redColor
            },

            {47 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},

            {50 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},
            {52 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},
            {
                54 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 2 * beatInterval,
                purpleColors[2]
            },
            {56 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[4]},
            {
                58 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval,
                purpleColors[1]
            },
            {60 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[4]},
            {
                62 * beatInterval + initialPlayerPositionX, 0.f, true, 3 * beatInterval, 1 * beatInterval,
                purpleColors[3]
            },
            {64 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[1]},
            {
                66 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval,
                purpleColors[3]
            },
            {
                68 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval,
                purpleColors[4]
            },
            {
                70 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval,
                purpleColors[3]
            },
            {
                72 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval,
                purpleColors[4]
            },
            {
                74 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval,
                purpleColors[1]
            },
            {
                76 * beatInterval + initialPlayerPositionX, 0.f, true, 3 * beatInterval, 1 * beatInterval,
                purpleColors[0]
            },
            {
                78 * beatInterval + initialPlayerPositionX, 0.f, true, 4 * beatInterval, 1 * beatInterval,
                purpleColors[1]
            },

            {80 * beatInterval + initialPlayerPositionX, 3.0f, true, 1 * beatInterval, 3 * beatInterval, blueColors[0]},
            {
                80 * beatInterval + initialPlayerPositionX + 0.25f * beatInterval, 3.0f + beatInterval, false,
                0.5f * beatInterval, 0.5f * beatInterval, redColor
            },
            {
                84 * beatInterval + initialPlayerPositionX, 4.0f, true, 1 * beatInterval, 2 * beatInterval,
                purpleColors[3]
            },
            {
                84.f * beatInterval + initialPlayerPositionX + 0.75f * beatInterval, 4.0f + beatInterval, false,
                0.5f * beatInterval, 0.5f * beatInterval, redColor
            },

            {86 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},

            {91 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[1]},
            {93 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},

            {98 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},
            {
                100 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval,
                purpleColors[0]
            },
            {102 * beatInterval + initialPlayerPositionX, 0.f, true, 3 * beatInterval, 1 * beatInterval, blueColors[3]},
            {104 * beatInterval + initialPlayerPositionX, 0.f, true, 4 * beatInterval, 1 * beatInterval, blueColors[3]},
            {106 * beatInterval + initialPlayerPositionX, 0.f, true, 5 * beatInterval, 1 * beatInterval, blueColors[1]},
            {
                108 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval,
                purpleColors[0]
            },
            {110 * beatInterval + initialPlayerPositionX, 0.f, true, 3 * beatInterval, 1 * beatInterval, blueColors[1]},
            {112 * beatInterval + initialPlayerPositionX, 0.f, true, 4 * beatInterval, 1 * beatInterval, blueColors[2]},
            {114 * beatInterval + initialPlayerPositionX, 0.f, true, 5 * beatInterval, 1 * beatInterval, blueColors[1]},
        };

        return game_objects;
    }

    void Game::onGameStateChange()
    {
        if (currentGameState == engine::GameState::Menu) return; //TODO implement Menu

        std::string audio_file = resolveAssetPath("audio/SensesShort.wav");

        unsigned int hopSize = 512; // Size of each hop
        unsigned int bufferSize = 2048; // Size of the analysis buffer

        bpm = engine::AudioAnalysis::analyzeAudioTempo(audio_file, hopSize, bufferSize);
        auto beatInterval = 60 / bpm;

        std::vector<float> beatPositions = engine::AudioAnalysis::generateBeatTimestamps(
            static_cast<float>(backgroundMusic->getLength()),
            60 / bpm,
            initialPlayerPositionX);

        levelLength = backgroundMusic->getLength();

        initial_test_game_objects = generateTestObjects(beatInterval, initialPlayerPositionX);

        if (currentGameState == engine::GameState::Level || currentGameState == engine::GameState::PreviewWithTesting ||
            currentGameState == engine::GameState::PreviewWithScrolling)
        {
            for (auto& object : initial_test_game_objects)
            {
                auto posY = object.positionY == 0
                                ? groundLevel + object.scaleY / 2
                                : groundLevel - object.scaleY / 2 + object.positionY * beatInterval;

                if (object.isPlatform)
                {
                    const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                        registry_, glm::vec3(object.positionX, posY, 0.0f),
                        object.color, "platform", physicsWorld);
                    engine::ecs::EntityFactory::setScale(registry_, entity,
                                                         glm::vec3(object.scaleX, object.scaleY, 0.f));
                    object.entityID = entity;
                    object.positionY = posY;
                }
                else
                {
                    const auto& entity = engine::ecs::EntityFactory::createDefaultEntity(
                        registry_, glm::vec3(object.positionX, posY, 0.0f),
                        object.color, "obstacle", physicsWorld, true);
                    engine::ecs::EntityFactory::setScale(registry_, entity,
                                                         glm::vec3(object.scaleX, object.scaleY, 0.f));
                    object.entityID = entity;
                    object.positionY = posY;
                }
            }

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

        if (currentGameState == engine::GameState::PreviewWithScrolling || currentGameState ==
            engine::GameState::PreviewWithTesting)
        {
            const auto timeLine = engine::ecs::EntityFactory::createDefaultEntity(
                registry_, glm::vec3(0, groundLevel, 0.0f), glm::vec4(0.1, 0.1, 1.0, 1.0f), "timeline", physicsWorld);
            engine::ecs::EntityFactory::setScale(registry_, timeLine, glm::vec3(40.f, 0.05f, 0.f));

            std::vector<GameObject> tempObjects;
            tempObjects.push_back(GameObject(0.f, groundLevel, true, 40.f, 0.05f, glm::vec4(0.1, 0.1, 1.0, 1.0f),
                                             timeLine));

            for (auto beatPosition : beatPositions)
            {
                auto timeLineColor = glm::vec4(0.1, 0.1, 1.0, 1.0f);
                const auto beat = engine::ecs::EntityFactory::createDefaultEntity(
                    registry_, glm::vec3(beatPosition, groundLevel, 0.0f), timeLineColor, "beat", physicsWorld);
                engine::ecs::EntityFactory::setScale(registry_, beat, glm::vec3(0.05f, 0.5f, 0.f));
                tempObjects.push_back(GameObject(beatPosition, groundLevel, true, 0.05f, 0.5f, timeLineColor, beat));
            }
            initial_test_game_objects = tempObjects;
        }

        if (currentGameState != engine::GameState::Menu && currentGameState != engine::GameState::PreviewWithScrolling)
        {
            const auto& entities = registry_.view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();

            for (auto& entity : entities)
            {
                auto& physics_comp = entities.get<engine::ecs::PhysicsComponent>(entity);
                auto& tag_comp = entities.get<engine::ecs::TagComponent>(entity);
                if (tag_comp.tag == "platform" || tag_comp.tag == "obstacle")
                {
                    b2Body_SetLinearVelocity(physics_comp.body, {levelSpeed * (60.0f / bpm / (60.0f / bpm)), 0.0f});
                }
            }
        }
        audio.playBackground(*backgroundMusic);
    }

    void Game::reset()
    {
        audio.stopAudioSource(*backgroundMusic);
        audio.playBackground(*backgroundMusic);

        // Reset all entities to their initial states
        resetComponents();
        if (currentGameState != engine::GameState::Menu && currentGameState != engine::GameState::PreviewWithScrolling)
        {
            const auto& entities = registry_.view<engine::ecs::TagComponent, engine::ecs::PhysicsComponent>();

            for (auto& entity : entities)
            {
                auto& physics_comp = entities.get<engine::ecs::PhysicsComponent>(entity);
                auto& tag_comp = entities.get<engine::ecs::TagComponent>(entity);
                if (tag_comp.tag == "platform" || tag_comp.tag == "obstacle")
                {
                    //b2Body_SetLinearVelocity(physics_comp.body, {levelSpeed, 0.0f});
                }
            }
        }
        isResetting = false;
    }

    void Game::resetComponents()
    {
        auto view = registry_.view<engine::ecs::TransformComponent, engine::ecs::TagComponent,
                                   engine::ecs::PhysicsComponent>();
        for (auto entity : view)

        {
            auto& transform = view.get<engine::ecs::TransformComponent>(entity);
            auto& tag = view.get<engine::ecs::TagComponent>(entity).tag;
            auto& physics_comp = view.get<engine::ecs::PhysicsComponent>(entity);
            b2Body_SetLinearVelocity(physics_comp.body, {0.f, 0.0f});

            if (tag == "player")
            {
                engine::ecs::EntityFactory::setPosition(registry_, entity,
                                                        glm::vec3(initialPlayerPositionX,
                                                                  groundLevel + transform.scale.y / 2, 0.f));
                continue;
            }
            if (tag == "ground")
            {
                engine::ecs::EntityFactory::setPosition(registry_, entity,
                                                        glm::vec3(0, groundLevel - groundHeight / 2, 0.0f));
                continue;
            }
            b2Body_SetLinearVelocity(physics_comp.body, {levelSpeed, 0.f});
            auto obj = std::ranges::find_if(initial_test_game_objects,
                                            [entity](const GameObject& o)
                                            {
                                                return o.entityID == entity;
                                            });
            engine::ecs::EntityFactory::setPosition(registry_, entity,
                                                    glm::vec3(obj->positionX, obj->positionY, 0.f));
        }
    }

    void Game::onPlayerDeath(engine::ecs::PlayerDeath& event)
    {
        std::cout << "Player died! Calling reset..." << std::endl;
        isResetting = true;
        reset();
    }

    Game::~Game()
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::PlayerDeath>().disconnect<&
            Game::onPlayerDeath>(this);
        glfwTerminate();
    }
}
