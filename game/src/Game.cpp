#include "Game.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include "entities/Planet.h"
#include "entities/Enemy.h"
#include "Assets.h"
#include "entities/Obstacle.h"
#include "entities/Platform.h"
#include "physics/ContactListener.h"

namespace gl3 {
    void Game::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    Game::Game(int width, int height, const std::string &title) {
        if(!glfwInit()) {
            throw std::runtime_error("Failed to initialize glfw");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if(window == nullptr) {
            throw std::runtime_error("Failed to create window");
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if(glGetError() != GL_NO_ERROR) {
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

    Game::~Game() {
        glfwTerminate();
    }

    glm::mat4 Game::calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 90.0f),
                                     glm::vec3(0.0f, 0.0f, 0.0),
                                     glm::vec3(0.0, 1.0, 0.0));

        glm::mat4 projection = glm::perspective(glm::radians(2.0f), 1000.0f / 600.0f, 0.1f, 100.0f);

        return projection * view * model;
    }

    void Game::run() {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        std::mt19937 randomNumberEngine{ static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()) };
        std::uniform_real_distribution positionDist{0.0, 2.0};
        std::uniform_real_distribution scaleDist{0.2, 0.6};
        std::uniform_real_distribution colorDist{0.0, 1.0};
        for(auto i = 0; i < 10; ++i) {
            if(i%2 != 0)
            {
            auto randomPosition = glm::vec3(i, -0.75f, 0);
            auto randomScale = static_cast<float>(scaleDist(randomNumberEngine));
            auto c = colorDist(randomNumberEngine);
            auto randomColor = glm::vec4( c, c, c, 1.0f);
            auto entity = std::make_unique<Platform>(randomPosition, randomScale, randomColor, physicsWorld);
            entities.push_back(std::move(entity));
            } else
            {
                auto position = glm::vec3(i, -0.875f, 0.0f);
                auto entity = std::make_unique<Obstacle>(position, 0.5f, glm::vec4(1, 1, 1, 1), physicsWorld);
                entities.push_back(std::move(entity));
            }

        }

        b2BodyDef bodyDef = b2DefaultBodyDef();
        b2BodyId groundId = b2CreateBody( physicsWorld, &bodyDef );
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2Segment segment = { { -20.0f, -1.0f }, { 20.0f, -1.0f } };
        b2CreateSegmentShape( groundId, &shapeDef, &segment );

        auto spaceShip = std::make_unique<Ship>(glm::vec3(-2, 0, 0), 0.0f, glm::vec3(0.25f, 0.25f, 0.25f), physicsWorld);
        ship = spaceShip.get();
        entities.push_back(std::move(spaceShip));

        /*auto enemy = std::make_unique<Enemy>(glm::vec3(2, 0, 0), 0, 0.1f, physicsWorld);
        entities.push_back(std::move(enemy));*/

        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(resolveAssetPath("audio/Senses.mp3").string().c_str());
        backgroundMusic->setLooping(true);
        audio.playBackground(*backgroundMusic);

        glfwSetTime(1.0 / 60);

        while(!glfwWindowShouldClose(window)) {
            update();
            updatePhysics();
            draw();
            updateDeltaTime();
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &VAO);
    }

    void Game::update() {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        for(auto &entity: entities) {
            entity->update(this, deltaTime);
        }
    }

    void Game::draw() {
        glClearColor(0.172f, 0.243f, 0.313f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(auto &entity: entities) {
            entity->draw(this);
        }

        glfwSwapBuffers(window);
    }

    void Game::updateDeltaTime() {
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
    }

    void Game::updatePhysics() {
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 4; // recommended sub-step count
        accumulator += deltaTime;
        if(accumulator >= fixedTimeStep){
            b2World_Step(physicsWorld, fixedTimeStep, subStepCount);
            ContactListener::checkForCollision(physicsWorld);

            // Update the entities based on what happened in the physics step
            for (const std::unique_ptr<Entity>& entity: entities) {
                if (dynamic_cast<Ship*>(entity.get()) == nullptr)
                {
                    b2Body_SetLinearVelocity( entity->getBody(), { -1.0f, 0.0f });
                }
                entity->updateBasedOnPhysics();
            }
            accumulator -= fixedTimeStep;
        }
    }

    const b2WorldId Game::getPhysicsWorld() const {
        return physicsWorld;
    }

}
