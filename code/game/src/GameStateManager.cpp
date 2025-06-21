#include "GameStateManager.h"

#include "engine/ecs/EntityFactory.h"
#include "engine/physics/PhysicsSystem.h"
#include "states/LevelSelectState.h"
#include "engine/stateManagement/StateManagerSystem.h"
#include "engine/userInterface/UISystem.h"
#include "states/LevelPlayState.h"

namespace gl3::game
{
    GameStateManager::GameStateManager(Game& game) : game_(game)
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().connect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().connect<&
            GameStateManager::onPauseLevel>(this);
        onUIInitHandle = game_.getUISystem()->onInitialized.addListener([this]()
        {
            onUiInitialized();
        });
    }

    GameStateManager::~GameStateManager()
    {
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::GameStateChange>().disconnect<&
            GameStateManager::onGameStateChange>(this);
        engine::ecs::EventDispatcher::dispatcher.sink<engine::ui::PauseLevelEvent>().disconnect<&
            GameStateManager::onPauseLevel>(this);
        game_.getUISystem()->onInitialized.removeListener(onUIInitHandle);
    }

    void GameStateManager::onUiInitialized() const
    {
        game_.getStateManagement()->pushState<state::LevelSelectState>(
            game_);
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

    void GameStateManager::onGameStateChange(const engine::ecs::GameStateChange& newState) const
    {
        auto* stateSystem = game_.getStateManagement();

        if (newState.newGameState == engine::GameState::LevelSelect)
        {
            game_.getPhysicsSystem()->setActive(false);
            stateSystem->changeState<state::LevelSelectState>(game_);
            return;
        }

        if (newState.newGameState == engine::GameState::Level && previous_game_state_ != engine::GameState::Level)
        {
            stateSystem->changeState<state::LevelPlayState>(game_, newState.newLevelIndex);
            return;
        }
    }

    void GameStateManager::onPauseLevel(const engine::ui::PauseLevelEvent& event) const
    {
        if (event.pauseLevel)
        {
            game_.getPhysicsSystem()->setActive(false);
            game_.getPlayerInputSystem()->setActive(false);
        }
        else
        {
            game_.getPhysicsSystem()->setActive(true);
            game_.getPlayerInputSystem()->setActive(true);
        }
    }
} // gl3
