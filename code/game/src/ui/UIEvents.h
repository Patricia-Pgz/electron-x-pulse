/**
* @file UIEvents.h
 * @brief Specialized Game UI events.
 */
#pragma once

namespace gl3::game::events{
    /**
     * Event for signaling that the in-game menu is now shown or hidden.
     */
    struct ShowGameMenu
    {
        bool showMenu = true;
    };

    /**
     * Event for signaling that the victory/finishing screen is now shown or hidden.
     */
    struct ShowFinishScreen
    {
        bool showScreen = true;
    };

}