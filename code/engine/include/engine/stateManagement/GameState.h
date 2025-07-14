#pragma once
namespace gl3::engine::state
{
    class GameState
    {
    public:
        explicit GameState(Game& game) : game(game){}
        virtual ~GameState() = default;
        virtual void update(float deltaTime) = 0;

        virtual void onEnter()
        {
        };

        virtual void onExit()
        {
        };

    protected:
        Game& game;
    };
}
