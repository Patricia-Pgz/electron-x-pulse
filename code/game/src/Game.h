#pragma once
#include "engine/Game.h"
#include <soloud_wav.h>
#include "entities/Player.h"

namespace gl3 {

    enum class GameState {
        Menu,
        Level,
        PreviewWithScrolling,
        PreviewWithTesting
    };

    struct GameObject
    {
        float positionX;
        float positionY;
        bool isPlatform;
        float scaleY;
        float scaleX;
        glm::vec4 color;
    };

    class Game: public engine::Game
    {
    public:
        Game(int width, int height, const std::string &title, glm::vec3 camPos, float camZoom);
        ~Game() override;
        [[nodiscard]] Player *getShip() const { return player; }
        float bpm = 0.0f; // Default BPM, updated dynamically //TODO not public
        float distancePerBeat = 2.0f; // Example: player travels 1 unit per beat

    private:
        void start() override;
        void update(GLFWwindow *window) override;
        void draw() override;
        void updatePhysics() override;
        void scroll_callback_fun(double yOffset);
        void onGameStateChange(); //TODO event
        void resetEntities();
        void reset();



        GameState previousGameState = GameState::Menu;
        GameState currentGameState = GameState::Menu;
        std::vector<std::unique_ptr<Entity>> entities;
        Player *player = nullptr;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        float levelLength = 0;
        float groundLevel = -1;
        float accumulator = 0.f;

        bool loadLevelFromFile = true;
        float levelSpeed = -1.f;

        float scrollSpeed = -1.f;
        bool isAudioPlaying = false;
        float initialPlayerPositionX = -2;
    };
}



