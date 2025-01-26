#include "Game.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include "Assets.h"
#include "entities/Obstacle.h"
#include "entities/Platform.h"
#include "physics/ContactListener.h"
#include "../aubio/src/aubio.h"

namespace gl3
{
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

    void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        auto gameInstance = static_cast<Game*>(glfwGetWindowUserPointer(window));
        glViewport(0, 0, width, height);
        gameInstance->calculateWindowBounds();
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) //TODO implement scrolling with Camera (+ limit to 0-width/2 and song/levellength+width/2
    {
        auto gameInstance = static_cast<Game*>(glfwGetWindowUserPointer(window));
        gameInstance->scroll_callback_fun(yoffset);
    }

    void Game::scroll_callback_fun(double yOffset)
    {
        float cameraX = cameraPosition.x;
        float scrollSpeed = 0.5f;
        float minScrollX = 0.0f; // Minimum scroll limit
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float maxScrollX = (levelLength + static_cast<float>(width)/2 *zoom - initialPlayerPositionX);  // Maximum scroll limit

        cameraX += static_cast<float>(yOffset) * scrollSpeed;

        if (cameraX < minScrollX) cameraX = minScrollX;
        if (cameraX > maxScrollX) cameraX = maxScrollX;

        cameraPosition.x = cameraX;
        cameraCenter.x = cameraX;
        calculateWindowBounds();
    }

    Game::Game(int width, int height, const std::string& title, glm::vec3 camPos,
               float camZoom): cameraPosition(camPos), zoom(camZoom)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize glfw");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr)
        {
            throw std::runtime_error("Failed to create window");
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glfwSetScrollCallback(window, scroll_callback);
        if (glGetError() != GL_NO_ERROR)
        {
            throw std::runtime_error("gl error");
        }
        calculateWindowBounds();
        audio.init();
        audio.setGlobalVolume(0.1f);

        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        // We use worldDef to define our physics world
        worldDef.gravity = b2Vec2{0.f, -9.81f};
        physicsWorld = b2CreateWorld(&worldDef);
    }

    Game::~Game()
    {
        glfwTerminate();
    }

    glm::mat4 Game::calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale)
    {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::lookAt(cameraPosition,
                                     cameraCenter,
                                     glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::ortho(windowLeft, windowRight, windowBottom, windowTop, 0.1f, 100.0f);

        return projection * view * model;
    }

    void Game::run()
    {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        auto groundHeight = 4.0f;
        auto groundPlatform = std::make_unique<Platform>(glm::vec3(0, groundLevel - groundHeight / 2, 0.0f), 40.0f,
                                                         groundHeight, glm::vec4(0.2, 0.8, 0.8, 1), physicsWorld);
        groundPlatform->setTag("ground");
        entities.push_back(std::move(groundPlatform));

        auto tempPlayer = std::make_unique<Player>(glm::vec3(initialPlayerPositionX, groundLevel + 0.25/2, 0), 0.0f,
                                                   glm::vec3(0.25f, 0.25f, 0.25f),
                                                   physicsWorld);
        player = tempPlayer.get();
        entities.push_back(std::move(tempPlayer));
        player->setOnDestroyedCallback([&]()
        {
            reset();
        });

        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(resolveAssetPath("audio/Senses.wav").c_str());
        backgroundMusic->setLooping(true);

        std::string audio_file = resolveAssetPath("audio/Senses.wav");
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
        std::uniform_real_distribution colorDist{0.2, 1.0};
        size_t index = 0;

        if (currentGameState == GameState::Level || currentGameState == GameState::PreviewWithTesting || currentGameState == GameState::PreviewWithScrolling)
        {
            int beatIndex = 1;
            for (auto beat : beatPositions)
            {
                if (index % 2 == 0)
                {
                    auto randomYScale = static_cast<float>(yScaleDist(randomNumberEngine));
                    auto randomXScale = static_cast<float>(xScaleDist(randomNumberEngine));
                    auto c = colorDist(randomNumberEngine);
                    auto height = beatIndex % 2 != 0? 0.25f: 0.5f;
                    auto randomColor = glm::vec4(0.1, c, c, 1.0f);
                    auto entity = std::make_unique<Platform>(glm::vec3(beat, groundLevel + height/2, 0.0f),0.5f, height,
                                                             randomColor, physicsWorld);
                    entities.push_back(std::move(entity));
                    beatIndex++;
                }
                else
                {
                    /*auto position = glm::vec3(beat, -0.875f, 0.0f);
                    auto entity = std::make_unique<
                        Obstacle>(position, 0.5f, glm::vec4(1.0, 0.1, 0.05, 1), physicsWorld);
                    entities.push_back(std::move(entity));*/
                }
                index++;
            }
        }

        if(currentGameState == GameState::PreviewWithScrolling || currentGameState == GameState::PreviewWithTesting)
        {
            auto horizontalLine = std::make_unique<Platform>(glm::vec3(0, groundLevel, 0.0f), 40,
                                                 0.05f, glm::vec4(0.1, 0.1, 1.0, 1.0f), physicsWorld, false);
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






        glfwSetTime(1.0 / 60);
        if(currentGameState != GameState::Menu && currentGameState != GameState::PreviewWithScrolling)
        {
            audio.playBackground(*backgroundMusic);

            for (const std::unique_ptr<Entity>& entity : entities)
            {
                if (entity->getTag() == "platform" || entity->getTag() == "obstacle")
                {
                    b2Body_SetLinearVelocity(entity->getBody(), {-1.0f, 0.0f});
                }
            }

        } else
        {
            b2Body_SetAwake(player->getBody(), false);
        }

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

    void Game::updateDeltaTime()
    {
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
    }

    void Game::updatePhysics()
    {
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 8; // recommended sub-step count
        accumulator += deltaTime;
        if (accumulator >= fixedTimeStep)
        {
            b2World_Step(physicsWorld, fixedTimeStep, subStepCount);
            ContactListener::checkForCollision(physicsWorld);

            if(currentGameState == GameState::PreviewWithScrolling) return;
            // Update the entities based on what happened in the physics step
            for (const std::unique_ptr<Entity>& entity : entities)
            {
                if (entity->getTag() == "timeline") continue;
                if (entity->getTag() == "beat")
                {
                    glm::vec3 position = entity->getPosition();

                    // Update position based on scroll speed and deltaTime
                    position.x += scrollSpeed * fixedTimeStep;

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

    void Game::calculateWindowBounds()
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float halfWidth = (width / 2.0f) * zoom;
        float halfHeight = (height / 2.0f) * zoom;

        windowLeft = cameraPosition.x - halfWidth;
        windowRight = cameraPosition.x + halfWidth;
        windowBottom = cameraPosition.y - halfHeight;
        windowTop = cameraPosition.y + halfHeight;

    }

    bool Game::isInVisibleWindow(const b2Vec2& position) const
    {
        float margin = 1.0f; // Optional margin
        return position.x >= (windowLeft - margin) &&
            position.x <= (windowRight + margin) &&
            position.y >= (windowBottom - margin) &&
            position.y <= (windowTop + margin);
    }

    b2WorldId Game::getPhysicsWorld() const
    {
        return physicsWorld;
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
        if(currentGameState != GameState::Menu && currentGameState != GameState::PreviewWithScrolling)
        {
            for (const std::unique_ptr<Entity>& entity : entities)
            {
                if (entity->getTag() == "platform" || entity->getTag() == "obstacle")
                {
                    b2Body_SetLinearVelocity(entity->getBody(), {-1.0f, 0.0f});
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
}
