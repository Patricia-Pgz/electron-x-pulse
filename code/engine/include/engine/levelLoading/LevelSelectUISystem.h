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
        bool is_active = false;
        int selectedLevel = -1;
        void DrawLevelButtons();
        void DrawLevelSelect(const ImGuiViewport* viewport, ImFont* font);
        void createLevelSelection();
    };
}
