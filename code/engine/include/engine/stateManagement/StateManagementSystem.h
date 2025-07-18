#pragma once
#include "GameState.h"
#include "engine/ecs/System.h"

namespace gl3::engine::state
{
    /**
     * @class StateManagementSystem
     * @brief System that manages the stack of active game states.
     *
     * This system allows pushing, popping, and switching between different states,
     * such as menus, gameplay, or pause screens, using a stack-based approach.
     * States are derived from the GameState base class.
     */
    class StateManagementSystem final : public ecs::System
    {
    public:
        /**
         * @brief Constructs the StateManagementSystem with a reference to the Game instance.
         * @param game Reference to the main Game instance.
         */
        explicit StateManagementSystem(Game& game) : System(game) {}

        /**
         * @brief Pushes a new state onto the state stack.
         *
         * The current active state will call its onExit() if @param exitCurrent is true.
         * The new state will be constructed in-place with the provided arguments and will call its onEnter().
         *
         * @tparam TState Type of the state to push. Must derive from GameState.
         * @tparam Args Variadic template arguments to forward to the state constructor.
         * @param exitCurrent If true, calls onExit() on the current active state.
         * @param args Arguments to forward to the new state's constructor.
         */
        template <typename TState, typename... Args>
        void pushState(const bool exitCurrent = true, Args&&... args)
        {
            static_assert(std::is_base_of_v<GameState, TState>, "TState must derive from GameState");

            if (exitCurrent && !states.empty())
                states.back()->onExit();

            states.push_back(std::make_unique<TState>(std::forward<Args>(args)...));
            states.back()->onEnter();
        }

        /**
         * @brief Replaces the current active state with a new one.
         *
         * Calls onExit() on the current state, removes it, and pushes the new state,
         * which will then call its onEnter().
         *
         * @tparam TState Type of the state to switch to. Must derive from GameState.
         * @tparam Args Variadic template arguments to forward to the state constructor.
         * @param args Arguments to forward to the new state's constructor.
         */
        template <typename TState, typename... Args>
        void changeState(Args&&... args)
        {
            static_assert(std::is_base_of_v<GameState, TState>, "TState must derive from GameState");

            if (!states.empty())
            {
                states.back()->onExit();
                states.pop_back();
            }

            states.push_back(std::make_unique<TState>(std::forward<Args>(args)...));
            states.back()->onEnter();
        }

        /**
         * @brief Pops the current active state from the stack.
         *
         * Calls onExit() on the popped state. If another state remains in the stack,
         * it will call onEnter() on it if @param enterNext is true.
         *
         * @param enterNext Whether to call onEnter() on the new top state after popping.
         */
        void popState(const bool enterNext = true)
        {
            if (!states.empty())
            {
                states.back()->onExit();
                states.pop_back();

                if (!states.empty() && enterNext)
                    states.back()->onEnter();
            }
        }

        /**
         * @brief Returns the active state from the top of the stack.
         *
         * @return Pointer to the current/top GameState, or nullptr if the stack is empty.
         */
        [[nodiscard]] GameState* getTopStateFromStack() const
        {
            return states.empty() ? nullptr : states.back().get();
        }

        /**
         * @brief Returns a const reference to the stack of states.
         *
         * @return Const reference to the vector of state pointers.
         */
        [[nodiscard]] const std::vector<std::unique_ptr<GameState>>& getStates() const
        {
            return states;
        }

        /**
         * @brief Updates all states in the stack.
         *
         * Calls update(deltaTime) on each state in the stack.
         */
        void update(const float deltaTime) const
        {
            for (const auto& state : getStates())
            {
                if (state)
                    state->update(deltaTime);
            }
        }

    private:
        /**
         * @brief Stack of active game states.
         */
        std::vector<std::unique_ptr<GameState>> states;
    };
}
