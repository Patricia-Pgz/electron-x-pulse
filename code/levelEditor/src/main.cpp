#include <iostream>
#include "LevelEditor.h"

int main() {
    try {
        gl3::LevelEditor levelEditor(1280, 720, "Level Editor", glm::vec3(0.0f, 0.0f, 1.0f),1.0/100);
        levelEditor.run();
    }
    catch(const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
}