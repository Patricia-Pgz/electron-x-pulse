#include "LevelEditorSystem.h"

#include <iostream>

namespace gl3 {

    void LevelEditorSystem::onTileSelected(TileSelectedEvent& event)
    {
        std::cout << "tile selected!";
    }
} // gl3