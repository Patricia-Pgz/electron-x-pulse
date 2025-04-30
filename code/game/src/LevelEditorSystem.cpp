#include "LevelEditorSystem.h"

#include <iostream>

namespace gl3 {

    void LevelEditorSystem::onTileSelected(TileSelectedEvent& event)
    {
        std::cout << "tile selected!"; //TODO platform/obstacle auswählen + dann erst event + entity erstellen + transform = mitte von ausgewähltem grid
    }
} // gl3