#include "Game.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include "Assets.h"
#include "entities/Obstacle.h"
#include "entities/Platform.h"
#include "physics/ContactListener.h"

namespace gl3
{
    void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    Game::Game(int width, int height, const std::string& title)
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
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (glGetError() != GL_NO_ERROR)
        {
            throw std::runtime_error("gl error");
        }

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
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 1.0f),
                                     glm::vec3(0.0f, 0.0f, 0.0),
                                     glm::vec3(0.0, 1.0, 0.0));

        // Switch to orthographic projection
        float left = -1280 / 2 / 100, right = 1280 / 2 / 100, bottom = -720 / 2 / 100, top = 720 / 2 / 100, nearPlane =
                  0.1f, farPlane = 100.0f;
        glm::mat4 projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane);

        return projection * view * model;
    }

    void Game::run()
    {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        std::mt19937 randomNumberEngine{
            static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
        };
        std::uniform_real_distribution yScaleDist{0.4, 1.1};
        std::uniform_real_distribution xScaleDist{0.8, 1.0};
        std::uniform_real_distribution colorDist{0.2, 1.0};
        for (auto i = 0; i < 40; ++i)
        {
            if (i % 2 != 0)
            {
                auto randomYScale = static_cast<float>(yScaleDist(randomNumberEngine));
                auto randomXScale = static_cast<float>(xScaleDist(randomNumberEngine));
                auto c = colorDist(randomNumberEngine);
                std::cout << std::to_string(randomXScale);
                auto randomColor = glm::vec4(0.1, c, c, 1.0f);
                auto entity = std::make_unique<Platform>(glm::vec3(i, groundLevel + randomYScale / 2, 0.0f), randomXScale, randomYScale,
                                                         randomColor, physicsWorld);
                entities.push_back(std::move(entity));
            }
            else
            {
                auto position = glm::vec3(i, -0.875f, 0.0f);
                auto entity = std::make_unique<Obstacle>(position, 0.5f, glm::vec4(1.0, 0.1, 0.05, 1), physicsWorld);
                entities.push_back(std::move(entity));
            }
        }
        auto groundHeight = 2.0f;
        auto groundPlatform = std::make_unique<Platform>(glm::vec3(0, groundLevel - groundHeight / 2, 0.0f), 40.0f,
                                                         groundHeight, glm::vec4(0.2, 0.8, 0.8, 1), physicsWorld);
        groundPlatform->setTag("ground");
        entities.push_back(std::move(groundPlatform));

        auto spaceShip = std::make_unique<Player>(glm::vec3(-2, 0, 0), 0.0f, glm::vec3(0.25f, 0.25f, 0.25f),
                                                physicsWorld);
        ship = spaceShip.get();
        entities.push_back(std::move(spaceShip));
        ship->setOnDestroyedCallback([&]()
        {
            reset();
        });

        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(resolveAssetPath("audio/Senses.mp3").string().c_str());
        backgroundMusic->setLooping(true);
        audio.playBackground(*backgroundMusic);

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

        for (auto& entity : entities)
        {
            entity->update(this, deltaTime);
        }
    }

    void Game::draw()
    {
        glClearColor(0.8f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& entity : entities)
        {
            entity->draw(this);
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
        const int subStepCount = 4; // recommended sub-step count
        accumulator += deltaTime;
        if (accumulator >= fixedTimeStep)
        {
            b2World_Step(physicsWorld, fixedTimeStep, subStepCount);
            ContactListener::checkForCollision(physicsWorld);

            // Update the entities based on what happened in the physics step
            for (const std::unique_ptr<Entity>& entity : entities)
            {
                if (entity->getTag() != "player" && entity->getTag() != "ground")
                {
                    b2Body_SetLinearVelocity(entity->getBody(), {-1.0f, 0.0f});
                }
                entity->updateBasedOnPhysics();
            }
            accumulator -= fixedTimeStep;
        }
    }

    b2WorldId Game::getPhysicsWorld() const
    {
        return physicsWorld;
    }

    void Game::reset()
    {
        std::cout << "Game reset!" << std::endl;

        audio.stopAudioSource(*backgroundMusic);
        audio.playBackground(*backgroundMusic);

        // Reset all entities to their initial states
        resetEntities();
        // Optionally reload the level or reinitialize other states
        // ...
    }

    void Game::resetEntities()
    {
        for (const std::unique_ptr<Entity>& entity : entities)
        {
            if(entity->getTag() != "ground")
            {
                entity->resetToInitialState();
            }
        }
    }
}
