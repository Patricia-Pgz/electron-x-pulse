#include <iostream>
#include "Game.h"

int main() {
    try {
        gl3::Game spaceBattle(1280, 720, "Space Battle");
        spaceBattle.run();
    }
    catch(const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
}