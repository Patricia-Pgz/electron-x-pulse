#include <iostream>
#include "Game.h"

int main() {
    try {
        gl3::game::Game musicPlatformer(1280, 720, "Music Platformer", glm::vec3(0.0f, 0.0f, 1.0f),1.0/100);
        musicPlatformer.run();
    }
    catch(const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
}