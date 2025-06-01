#pragma once
#include "InGameMenuSystem.h"
#include "InstructionUI.h"
#include "engine/levelEditor/EditorUISystem.h"
#include "engine/levelLoading/LevelSelectUISystem.h"
#include "engine/userInterface/UISystem.h"

namespace gl3::game::ui
{
    class TopLvlUISystem : public engine::ui::UISystem
    {
    public:
        explicit TopLvlUISystem(Game& game) : UISystem(game){};

        void initSubsystems() override
        {
            //addSubsystem(std::make_unique<inGameMenu::InGameMenuSystem>(imgui_io, game));
           // addSubsystem(std::make_unique<TutorialUI>(imgui_io, game));
           // addSubsystem(std::make_unique<editor::EditorUISystem>(imgui_io, game));
            addSubsystem(std::make_unique<engine::levelLoading::LevelSelectUISystem>(imgui_io, game));
        };
    };
}
