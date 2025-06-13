#pragma once
namespace gl3::engine::state
{
    class GameState
    {
    public:
        //TODO im Constructor game& mitgeben!
        virtual ~GameState() = default;
        virtual void update(float dt) = 0;

        virtual void onEnter()
        {
        };

        virtual void onExit()
        {
        };
    };
}
