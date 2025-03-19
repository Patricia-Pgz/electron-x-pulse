#include "Game.h"
#include <random>
#include <iostream>
#include "Assets.h"
#include "entities/Obstacle.h"
#include "entities/Platform.h"
#include "physics/ContactListener.h"
#include "engine/AudioAnalysis.h"
#include "engine/ecs/EntityFactory.h"
#include "engine/rendering/RenderingSystem.h"

namespace gl3
{

    Game::Game(int width, int height, const std::string &title, glm::vec3 camPos, float camZoom)
        : engine::Game(width, height, title, camPos, camZoom) {
    }

    void Game::scroll_callback_fun(const double yOffset)
    {
        if(currentGameState != GameState::PreviewWithScrolling) return;
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

        for (auto entity : entities) {
            auto& tag_component = entities.get<engine::ecs::TagComponent>(entity);
            auto& physics_component = entities.get<engine::ecs::PhysicsComponent>(entity);
            if(tag_component.tag != "beat" && tag_component.tag != "timeline")
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
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        context.onScrolling.addListener([&](const float offsetY) {
           scroll_callback_fun(offsetY);
        });
        auto groundHeight = 4.0f;
        auto groundPlatform = std::make_unique<Platform>(glm::vec3(0, groundLevel - groundHeight / 2, 0.0f), 40.0f,
                                                         groundHeight, glm::vec4(0.25, 0.27, 1, 1), physicsWorld);
        groundPlatform->setTag("ground");

        player = std::make_unique<entt::entity>(engine::ecs::EntityFactory::createDefaultEntity(registry_,glm::vec3(initialPlayerPositionX, groundLevel + 0.25 / 2, 0), glm::vec3(0.25f, 0.25f, 0.25f), 0.f,glm::vec4(0.25f, 0.25f, 0.25f, 1.0f), "player")) ;
        /*player->onPlayerDeath.addListener([&] { //TODO System player events/functionality?
           reset();
        }); //save this handle if I want to unsubscribe later*/
        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(resolveAssetPath("audio/SensesShort.wav").c_str());
        backgroundMusic->setLooping(false);

        //glDeleteVertexArrays(1, &VAO);
    }

    void Game::update(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            currentGameState = GameState::Level;
        }

        if (currentGameState != previousGameState)
        {
            previousGameState = currentGameState;
            onGameStateChange();
        }
        auto entities = registry_.view<engine::ecs::TagComponent, engine::ecs::TransformComponent>();

        for (auto& entity : entities)
        {
            if (context.isInVisibleWindow(entities.get<engine::ecs::TransformComponent>(entity).position))
            {
                //entity->update(this, deltaTime); //TODO hatte noch jemand außer Player wirklich was in update? -> System für playermovement
            }
        }
    }

    void Game::draw()
    {
        engine::rendering::RenderingSystem::draw(registry_,context); //TODO mesh und shader in rendering namespace
    }

    //TODO:
    /*Entity::~Entity()
    {
        if (b2World_IsValid(physicsWorld))
        {
            if (b2Body_IsValid(body))
                b2DestroyBody(body); //TODO does this get called when the physicscomponent gets deleted? -> levelReload/Load/wechsel
        }
    }

    void Entity::updateBasedOnPhysics() //TODO physics system
    {
        if (!b2Body_IsValid(body))
            return;

        auto physicsTransform = b2Body_GetTransform(body);

        position.x = physicsTransform.p.x;
        position.y = physicsTransform.p.y;

        zRotation = glm::degrees(b2Rot_GetAngle(physicsTransform.q));
    }

    void Entity::resetToInitialState()
    {
        position = initialPosition;
        zRotation = initialZRotation;
        scale = initialScale;

        if (const auto body = getBody(); b2Body_IsValid(body))
        {
            b2Body_SetTransform(body, {position.x, position.y}, b2MakeRot(glm::radians(initialZRotation)));
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
            b2Body_SetAngularVelocity(body, 0.0f);
        }
    }*/

    void Game::updatePhysics()
    {
        if (currentGameState == GameState::Menu) return;
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 8; // recommended sub-step count
        accumulator += deltaTime;
        if (accumulator >= fixedTimeStep)
        {
            b2World_Step(physicsWorld, fixedTimeStep, subStepCount);
            ContactListener::checkForCollision(physicsWorld);

            if (currentGameState == GameState::PreviewWithScrolling) return;
            // Update the entities based on what happened in the physics step
            for (const std::unique_ptr<Entity>& entity : entities)
            {
                if (entity->getTag() == "timeline") continue;
                if (entity->getTag() == "beat")
                {
                    glm::vec3 position = entity->getPosition();

                    // Update position based on scroll speed and deltaTime
                    position.x += levelSpeed * fixedTimeStep;

                    // Apply the new position to the entity
                    entity->setPosition(position);
                    continue;
                }
                auto physicalPos = b2Body_GetPosition(entity->getBody());
                auto entityPos = glm::vec2(physicalPos.x, physicalPos.y);

                /*if (entity->getTag() != "player" && entity->getTag() != "ground")
                {
                    b2Body_SetLinearVelocity(entity->getBody(), {0.5f, 0.0f});
                }*/
                if (context.isInVisibleWindow(entityPos))
                {
                    entity->updateBasedOnPhysics();
                }
                // If entity is far off-screen, sleep it to save performance
                if (b2Body_GetPosition(entity->getBody()).x < context.getWindowBounds()[0] - 1.0f)
                // Arbitrary value to check if the entity is far off-screen
                {
                    b2Body_SetAwake(entity->getBody(), false); // Sleep the body
                }
                else
                {
                    b2Body_SetAwake(entity->getBody(), true); // Wake the body if it’s back on screen
                }
            }
            accumulator -= fixedTimeStep;
        }
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
        glm::vec4 redColor{1.f,0.f,0.f,1.f};

        size_t index = 0;

        double previousC = 0.0;

        for (int i = 0; i <= 5; i++)
        {
            auto c = blueColorDist(randomNumberEngine);
            while(c == previousC){c = blueColorDist(randomNumberEngine);}
            blueColors.push_back(glm::vec4(0.16f, 0.1f, c, 1.0f));
            purpleColors.push_back(glm::vec4(c, 0.f, 0.2, 1.0f));
            previousC = c;
        }
        std::vector<GameObject> game_objects = {
            {4 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval,1 * beatInterval, blueColors[0]},
            {5 * beatInterval + initialPlayerPositionX, 0.f, false,0.8f * beatInterval,  1 * beatInterval,  redColor},

            {11 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval,  2 * beatInterval, blueColors[1]},

            {16 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, purpleColors[0]},
            {18 * beatInterval + initialPlayerPositionX, 0.f, true, 2 * beatInterval, 1 * beatInterval, purpleColors[1]},
            {20 * beatInterval + initialPlayerPositionX, 0.f, true, 3 * beatInterval, 1 * beatInterval, purpleColors[0]},
            {22 * beatInterval + initialPlayerPositionX, 0.f, true, 4 * beatInterval, 1 * beatInterval, purpleColors[1]},

            {28 * beatInterval + initialPlayerPositionX, 0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},
            {28 * beatInterval + initialPlayerPositionX + 0.75f * beatInterval, 0.f, false,   0.5f * beatInterval,1 * beatInterval, redColor},
                {28 * beatInterval + initialPlayerPositionX+ 1.25f * beatInterval, 0.f, false, 0.5f * beatInterval, 1 * beatInterval, redColor},
            {30 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},
            {32 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},

            {38 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, purpleColors[1]},
{39 * beatInterval + initialPlayerPositionX, 0.f, false,0.6f * beatInterval,  1 * beatInterval,  redColor},
            {40 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, purpleColors[3]},
{41 * beatInterval + initialPlayerPositionX, 0.f, false,0.6f * beatInterval,  1 * beatInterval,  redColor},
            {42 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, purpleColors[1]},
{43 * beatInterval + initialPlayerPositionX, 0.f, false,0.6f * beatInterval,  1 * beatInterval,  redColor},

{47 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},

{50 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},
{52 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},
            {54 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 2 * beatInterval, purpleColors[2]},
            {56 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[4]},
            {58 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, purpleColors[1]},
            {60 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[4]},
            {62 * beatInterval + initialPlayerPositionX,0.f, true, 3 * beatInterval, 1 * beatInterval, purpleColors[3]},
            {64 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[1]},
            {66 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, purpleColors[3]},
            {68 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, purpleColors[4]},
{70 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, purpleColors[3]},
            {72 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, purpleColors[4]},
            {74 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, purpleColors[1]},
            {76 * beatInterval + initialPlayerPositionX,0.f, true, 3 * beatInterval, 1 * beatInterval, purpleColors[0]},
            {78 * beatInterval + initialPlayerPositionX,0.f, true, 4 * beatInterval, 1 * beatInterval, purpleColors[1]},

{80 * beatInterval + initialPlayerPositionX,3.0f, true, 1 * beatInterval, 3 * beatInterval, blueColors[0]},
{80 * beatInterval + initialPlayerPositionX + 0.25f*beatInterval,3.0f+beatInterval, false, 0.5f * beatInterval, 1 * beatInterval, redColor},
            {84 * beatInterval + initialPlayerPositionX,4.0f, true, 1 * beatInterval, 2 * beatInterval, purpleColors[3]},
{84.f * beatInterval + initialPlayerPositionX + 0.75f * beatInterval,4.0f + beatInterval, false, 0.5f * beatInterval, 1 * beatInterval, redColor},

            {86 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},

            {91 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[1]},
            {93 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, blueColors[1]},

            {98 * beatInterval + initialPlayerPositionX,0.f, true, 1 * beatInterval, 1 * beatInterval, blueColors[0]},
            {100 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, purpleColors[0]},
            {102 * beatInterval + initialPlayerPositionX,0.f, true, 3 * beatInterval, 1 * beatInterval, blueColors[3]},
            {104 * beatInterval + initialPlayerPositionX,0.f, true, 4 * beatInterval, 1 * beatInterval, blueColors[3]},
            {106 * beatInterval + initialPlayerPositionX,0.f, true, 5 * beatInterval, 1 * beatInterval, blueColors[1]},
            {108 * beatInterval + initialPlayerPositionX,0.f, true, 2 * beatInterval, 1 * beatInterval, purpleColors[0]},
            {110 * beatInterval + initialPlayerPositionX,0.f, true, 3 * beatInterval, 1 * beatInterval, blueColors[1]},
            {112 * beatInterval + initialPlayerPositionX,0.f, true, 4 * beatInterval, 1 * beatInterval, blueColors[2]},
            {114 * beatInterval + initialPlayerPositionX,0.f, true, 5 * beatInterval, 1 * beatInterval, blueColors[1]},
        };

        return game_objects;
    }

    void Game::onGameStateChange()
    {
        if (currentGameState == GameState::Menu) return; //TODO implement Menu

        std::string audio_file = resolveAssetPath("audio/SensesShort.wav");
        std::vector<float> beatPositions;

        unsigned int hopSize = 512; // Size of each hop
        unsigned int bufferSize = 2048; // Size of the analysis buffer

        bpm = engine::AudioAnalysis::analyzeAudioTempo(audio_file,hopSize,bufferSize);
        auto beatInterval = 60 / bpm;

        beatPositions = engine::AudioAnalysis::generateBeatTimestamps(static_cast<float>(backgroundMusic->getLength()), 60 / bpm,
                                               initialPlayerPositionX);
        levelLength = backgroundMusic->getLength();

        std::vector<GameObject> game_objects = generateTestObjects(beatInterval, initialPlayerPositionX);

        if (currentGameState == GameState::Level || currentGameState == GameState::PreviewWithTesting ||
            currentGameState == GameState::PreviewWithScrolling)
        {
            for (auto object : game_objects)
            {
                auto posY = object.positionY == 0? groundLevel + object.scaleY / 2 : groundLevel - object.scaleY / 2 + object.positionY * beatInterval;

                if (object.isPlatform)
                {
                    auto entity = std::make_unique<Platform>(
                        glm::vec3(object.positionX, posY, 0.0f), object.scaleX,
                        object.scaleY,
                        object.color, physicsWorld);
                    entities.push_back(std::move(entity));
                }
                else
                {
                    auto entity = std::make_unique<Obstacle>(
                        glm::vec3(object.positionX, posY, 0.0f),
                        object.scaleY,
                        object.color, physicsWorld);
                    entities.push_back(std::move(entity));
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
                    auto entity = std::make_unique<Platform>(glm::vec3(beat, groundLevel + height / 2, 0.0f), 0.5f,
                                                             height,
                                                             randomColor, physicsWorld);
                    entities.push_back(std::move(entity));
                }
                else
                {
                    auto position = glm::vec3(beat, -0.875f, 0.0f);
                    auto entity = std::make_unique<
                        Obstacle>(position, 0.5f, glm::vec4(1.0, 0.1, 0.05, 1), physicsWorld);
                    entities.push_back(std::move(entity));
                }
                index++;
            }*/
        }

        if (currentGameState == GameState::PreviewWithScrolling || currentGameState == GameState::PreviewWithTesting)
        {
            auto horizontalLine = std::make_unique<Platform>(glm::vec3(0, groundLevel, 0.0f), 40,
                                                             0.05f, glm::vec4(0.1, 0.1, 1.0, 1.0f), physicsWorld,
                                                             false);
            horizontalLine->setTag("timeline");
            entities.push_back(std::move(horizontalLine));

            for (auto beatPosition : beatPositions)
            {
                auto timeLineColor = glm::vec4(0.1, 0.1, 1.0, 1.0f);
                auto entity = std::make_unique<Platform>(glm::vec3(beatPosition, groundLevel, 0.0f), 0.05f, 0.5f,
                                                         timeLineColor, physicsWorld, false);
                entity->setTag("beat");
                entities.push_back(std::move(entity));
            }
        }

        if (currentGameState != GameState::Menu && currentGameState != GameState::PreviewWithScrolling)
        {
            for (const std::unique_ptr<Entity>& entity : entities)
            {
                if (entity->getTag() == "platform" || entity->getTag() == "obstacle")
                {
                    b2Body_SetLinearVelocity(entity->getBody(), {levelSpeed, 0.0f});
                }
            }
        }
        else
        {
            b2Body_SetAwake(player->getBody(), false);
        }
        audio.playBackground(*backgroundMusic);
    }

    void Game::reset()
    {
        std::cout << "Game reset!" << std::endl;

        audio.stopAudioSource(*backgroundMusic);
        audio.playBackground(*backgroundMusic);

        // Reset all entities to their initial states
        resetEntities();
        if (currentGameState != GameState::Menu && currentGameState != GameState::PreviewWithScrolling)
        {
            for (const std::unique_ptr<Entity>& entity : entities)
            {
                if (entity->getTag() == "platform" || entity->getTag() == "obstacle")
                {
                    b2Body_SetLinearVelocity(entity->getBody(), {levelSpeed, 0.0f});
                }
            }
        }

        // Optionally reload the level or reinitialize other states
        // ...
    }

    void Game::resetEntities()
    {
        for (const std::unique_ptr<Entity>& entity : entities)
        {
            entity->resetToInitialState();
        }
    }

    Game::~Game()
    {
        glfwTerminate();
    }
}
