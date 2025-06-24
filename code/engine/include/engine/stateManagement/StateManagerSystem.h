#pragma once
#include "GameState.h"
#include "engine/ecs/System.h"

namespace gl3::engine::state
{
    class StateManagementSystem final : public ecs::System
    {
    public:
        explicit StateManagementSystem(Game& game) : System(game)
        {
        };

        template <typename TState, typename... Args>
        void pushState(const bool exitCurrent = true, Args&&... args)
        {
            static_assert(std::is_base_of_v<GameState, TState>, "TState must derive from GameState");

            if (exitCurrent && !states.empty())
                states.back()->onExit();

            states.push_back(std::make_unique<TState>(std::forward<Args>(args)...));
            states.back()->onEnter();
        }

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

        [[nodiscard]] GameState* getCurrentState() const
        {
            return states.empty() ? nullptr : states.back().get();
        }

        [[nodiscard]] const std::vector<std::unique_ptr<GameState>>& getStates() const
        {
            return states;
        }

        void update() const
        {
            const float deltaTime = game_.getDeltaTime();

            for (const auto& state : getStates())
            {
                if (state)
                    state->update(deltaTime);
            }
        }

    private:
        std::vector<std::unique_ptr<GameState>> states;
    };
}
