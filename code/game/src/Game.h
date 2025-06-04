#pragma once
#include "engine/Game.h"
#include <soloud_wav.h>
#include "PlayerInputSystem.h"

namespace gl3::game
{
    class GameStateManager;

    struct GameObject
    {
        float positionX;
        float positionY;
        bool isPlatform;
        float scaleY;
        float scaleX;
        glm::vec4 color;
        entt::entity entityID;
    };

    struct AudioBundle
    {
        SoLoud::Soloud& audio;
        SoLoud::Wav& backgroundMusic;
    };

    class Game final : public engine::Game
    {
    public:
        Game(int width, int height, const std::string& title, const glm::vec3& camPos, float camZoom);
        ~Game() override;
        AudioBundle getAudioAndHandle() { return {audio_, *backgroundMusic}; }

    private:
        void start() override;
        void update(GLFWwindow* window) override;
        void registerUiSystems() override;
        void moveEntitiesScrolling();
        void on_mouse_scroll(engine::context::onMouseScrollEvent& event);

        GameStateManager* game_state_manager_;
        input::PlayerInputSystem player_input_system_;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;


        float unit = 1.f;

        bool loadLevelFromFile = true;
    };
}
