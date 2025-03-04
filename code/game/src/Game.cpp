#include "Game.h"
#include <random>
#include <iostream>
#include "Assets.h"
#include "entities/Obstacle.h"
#include "entities/Platform.h"
#include "physics/ContactListener.h"
#include "../aubio/src/aubio.h"

namespace gl3
{

    Game::Game(int width, int height, const std::string &title, glm::vec3 camPos, float camZoom)
        : engine::Game(width, height, title, camPos, camZoom) {
    }

    std::vector<float> generateBeatTimestamps(const float songLength, const float beatInterval, float& offset)
    {
        std::vector<float> beatTimestamps;

        // Ensure valid inputs
        if (songLength <= 0.0f || beatInterval <= 0.0)
        {
            std::cerr << "Invalid song length or beat interval!" << std::endl;
            return beatTimestamps;
        }

        // Convert song length to the number of beats
        const int totalBeats = static_cast<int>(songLength / beatInterval);

        // Generate timestamps
        for (int i = 0; i <= totalBeats; ++i)
        {
            beatTimestamps.push_back(static_cast<float>(i) * beatInterval + offset);
        }

        return beatTimestamps;
    }

    void Game::scroll_callback_fun(double yOffset)
    {
        float cameraX = 0.0f; /*cameraPosition.x;*/
        float scrollSpeed = 0.5f;
        float minScrollX = 0.0f; // Minimum scroll limit
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float maxScrollX = (levelLength + static_cast<float>(width) / 2 * zoom - initialPlayerPositionX);
        // Maximum scroll limit

        cameraX = static_cast<float>(yOffset) * scrollSpeed;
        if (cameraX < minScrollX) cameraX = minScrollX;
        if (cameraX > maxScrollX) cameraX = maxScrollX;


        for (auto& entity : entities)
        {
            if (entity->getTag() != "beat" && entity->getTag() != "timeline")
            {
                b2Body_SetTransform(entity->getBody(),
                                    b2Vec2(b2Body_GetPosition(entity->getBody()).x + cameraX,
                                           b2Body_GetPosition(entity->getBody()).y),
                                    b2Body_GetRotation(entity->getBody()));
            }
            else
            {
                entity->setPosition(glm::vec3(entity->getPosition().x + cameraX, entity->getPosition().y, 0.0f));
            }
        }

        /*cameraPosition.x = cameraX;
        cameraCenter.x = cameraX;
        calculateWindowBounds();*/
    }

    void Game::run()
    {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        auto groundHeight = 4.0f;
        auto groundPlatform = std::make_unique<Platform>(glm::vec3(0, groundLevel - groundHeight / 2, 0.0f), 40.0f,
                                                         groundHeight, glm::vec4(0.25, 0.27, 1, 1), physicsWorld);
        groundPlatform->setTag("ground");
        entities.push_back(std::move(groundPlatform));

        auto tempPlayer = std::make_unique<Player>(glm::vec3(initialPlayerPositionX, groundLevel + 0.25 / 2, 0), 0.0f,
                                                   glm::vec3(0.25f, 0.25f, 0.25f),
                                                   physicsWorld);
        player = tempPlayer.get();
        entities.push_back(std::move(tempPlayer));
        player->onPlayerDeath.addListener([&] {
           reset();
        }); //save this handle if I want to unsubscribe later
        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(resolveAssetPath("audio/SensesShort.wav").c_str());
        backgroundMusic->setLooping(false);

        glfwSetTime(1.0 / 60);

        while (!glfwWindowShouldClose(window))
        {
            update();
            updatePhysics();
            draw();
            updateDeltaTime();
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &VAO);
    }

    void Game::update()
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

        for (auto& entity : entities)
        {
            if (isInVisibleWindow(b2Vec2(entity->getPosition().x, entity->getPosition().y)))
            {
                entity->update(this, deltaTime);
            }
        }
    }

    void Game::draw()
    {
        glClearColor(0.8f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (const auto& entity : entities)
        {
            if (isInVisibleWindow(b2Vec2(entity->getPosition().x, entity->getPosition().y)))
            {
                entity->draw(this);
            }
        }

        glfwSwapBuffers(window);
    }

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
                /*if (entity->getTag() != "player" && entity->getTag() != "ground")
                {
                    b2Body_SetLinearVelocity(entity->getBody(), {0.5f, 0.0f});
                }*/
                if (isInVisibleWindow(b2Body_GetPosition(entity->getBody())))
                {
                    entity->updateBasedOnPhysics();
                }
                // If entity is far off-screen, sleep it to save performance
                if (b2Body_GetPosition(entity->getBody()).x < windowLeft - 1.0f)
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

    bool Game::isInVisibleWindow(const b2Vec2& position) const
    {
        float margin = 1.0f; // Optional margin
        return position.x >= (windowLeft - margin) &&
            position.x <= (windowRight + margin) &&
            position.y >= (windowBottom - margin) &&
            position.y <= (windowTop + margin);
    }

    void Game::onGameStateChange()
    {
        if (currentGameState == GameState::Menu) return; //TODO implement Menu

        std::string audio_file = resolveAssetPath("audio/SensesShort.wav");
        std::vector<float> beatPositions;

        uint_t hop_size = 512; // Size of each hop
        uint_t buffer_size = 2048; // Size of the analysis buffer
        uint_t samplerate = 0; // Will be set by the file

        // Create aubio source object
        aubio_source_t* source = new_aubio_source(audio_file.c_str(), samplerate, hop_size);
        if (!source)
        {
            std::cerr << "Error: Failed to open audio source!" << std::endl;
        }

        // Get actual samplerate
        samplerate = aubio_source_get_samplerate(source);

        // Create aubio tempo object
        aubio_tempo_t* tempo = new_aubio_tempo("complex", buffer_size, hop_size, samplerate);
        if (!tempo)
        {
            std::cerr << "Error: Failed to create tempo object!" << std::endl;
            del_aubio_source(source);
        }

        fvec_t* audio_frame = new_fvec(hop_size); // Audio input frame buffer
        fvec_t* beat_output = new_fvec(1); // Beat detection output buffer

        uint_t read = 0;
        while (true)
        {
            // Read a frame of audio
            aubio_source_do(source, audio_frame, &read);

            // Stop if no more frames
            if (read == 0) break;

            aubio_tempo_set_threshold(tempo, 0.9);

            // Analyze for beats
            aubio_tempo_do(tempo, audio_frame, beat_output);

            /*// Check if a beat was detected TODO als andere beatdetection anbieten
            if (fvec_get_sample(beat_output, 0) > 0.0f) {
                float beatTime = aubio_tempo_get_last_s(tempo);
                float beatPosition = initialPlayerPositionX + beatTime * -scrollSpeed;
                beatPositions.push_back(beatPosition);
                std::cout << "Beat detected at X position: " << beatPosition << std::endl;
            }*/
        }
        bpm = aubio_tempo_get_bpm(tempo);
        beatPositions = generateBeatTimestamps(static_cast<float>(backgroundMusic->getLength()), 60 / bpm,
                                               initialPlayerPositionX);
        levelLength = backgroundMusic->getLength();


        del_fvec(beat_output);
        del_fvec(audio_frame);
        del_aubio_tempo(tempo);
        del_aubio_source(source);

        aubio_cleanup();

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

        auto beatInterval = 60 / bpm;
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
            /*for (auto beat : beatPositions)
            {
                if (index < 1)
                {
                    index++;
                    continue;
                }

                if (index % 2 == 0)
                {
                    auto randomYScale = static_cast<float>(yScaleDist(randomNumberEngine));
                    auto randomXScale = static_cast<float>(xScaleDist(randomNumberEngine));
                    auto c = colorDist(randomNumberEngine);
                    auto height = 0.25f;
                    auto randomColor = glm::vec4(0.1, c, c, 1.0f);
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



    void Game::setCameraPosition(const glm::vec3& position)
    {
        cameraPosition = position;
    }

    void Game::setZoom(float newZoom)
    {
        zoom = newZoom;
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
