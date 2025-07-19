/**
* @file main.cpp
 * @brief Initializes the Game and starts its update loop.
 */
#include <iostream>
#include "Game.h"

int main()
{
    try
    {
        /// Create the main game instance with window size, title, camera position, and zoom level. (Zooming after initialization not implemented in context, leave it at 1/100)
        gl3::game::Game ElectronXPulse(
            1280, // Window width
            720, // Window height
            "ElectronXPulse", // Window title
            glm::vec3(0.0f, 0.0f, 1.0f), // Initial camera position
            1.0 / 100.f // Camera zoom standard value
        );

        /// Run the main game loop. (Could call start() before this, but don't need to)
        ElectronXPulse.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
}
