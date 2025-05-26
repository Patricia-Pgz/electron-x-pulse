#pragma once
#include <engine/userInterface/IUISubSystem.h>

namespace gl3::engine::levelLoading
{
    ///summary
    ///Level selection UI.
    ///summary
    class LevelSelectUISystem : public ui::IUISubsystem
    {
    public:
        explicit LevelSelectUISystem(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game)
        {
        };
        void update() override;

    private:
        static void createLevelSelection();
    };
}
