#pragma once
#include "engine/Game.h"

namespace gl3::engine::ecs
{
    /**
  * @class System
  * @brief Base class for all ECS (Entity-Component-System) systems.
  *
  * A System performs logic or processing on entities and components
  * within the game engine. All specific systems (like rendering,
  * physics, audio) should inherit from this base class.
  *
  * Systems can be activated or deactivated using setActive().
  */
    class System {
    public:
        /**
         * @brief Construct a new System.
         * @param game_ Reference to the main Game instance.
         */
        explicit System(Game &game_) : game(game_) {}

        // Disable copy and move semantics
        System(System &&) = delete;
        System(const System &) = delete;

        /**
         * @brief Virtual destructor.
         */
        virtual ~System() = default;

        /**
         * @brief Activate or deactivate the system.
         * @param setActive True to activate, false to deactivate.
         */
        void setActive(const bool setActive)
        {
            is_active = setActive;
        }

    protected:
        Game &game;      /**< Reference to the main Game instance. */
        bool is_active = true; /**< Whether the system is currently active. */
    };

}
