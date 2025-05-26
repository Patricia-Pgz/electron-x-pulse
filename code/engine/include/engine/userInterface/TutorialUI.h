#pragma once
#include "engine/Game.h"
#include "IUISubSystem.h"

namespace gl3::engine::ui {

class TutorialUI : public IUISubsystem{
public:
    explicit TutorialUI(ImGuiIO* imguiIO, Game& game) : IUISubsystem(imguiIO, game){}
    void update() override;
private:
    void DrawHints(const ImGuiViewport* viewport, ImFont* font);
    bool show_hints_ = true;
    static constexpr ImGuiWindowFlags flags_ =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBringToFrontOnFocus;
};

} // gl3

