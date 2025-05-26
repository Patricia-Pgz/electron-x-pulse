#pragma once
#include "InGameMenuSystem.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "engine/userInterface/UISystem.h"

namespace gl3::engine::ui
{
    class TopLvlUISystem : public UISystem
    {
    public:
        explicit TopLvlUISystem(Game& game) : UISystem(game)
        {
            //addSubsystem(std::make_unique<inGameUI::InGameMenuSystem>(imgui_io, game));
            addSubsystem(std::make_unique<editor::EditorUISystem>(imgui_io, game)); //TODO kaputt
            //addSubsystem(std::make_unique<levelLoading::LevelSelectUISystem>(imgui_io));
        };
    };
}
