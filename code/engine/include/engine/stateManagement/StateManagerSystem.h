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
        void pushState(Args&&... args)
        {
            static_assert(std::is_base_of_v<GameState, TState>, "TState must derive from GameState");

            if (!states.empty())
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

        void popState()
        {
            if (!states.empty())
            {
                states.back()->onExit();
                states.pop_back();

                if (!states.empty())
                    states.back()->onEnter();
            }
        }

        [[nodiscard]] GameState* getCurrentState() const
        {
            return states.empty() ? nullptr : states.back().get();
        }

        void update() const
        {
            if (auto* state = getCurrentState())
                state->update(game.getDeltaTime());
        }

    private:
        std::vector<std::unique_ptr<GameState>> states;
    };
}
