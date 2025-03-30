#pragma once
#include "box2d/box2d.h"
#include "engine/Game.h"
#include "engine/ecs/System.h"

namespace gl3
{
    struct JumpConfig
    {
        float gravity;
        float bpm;
        float beatsPerJump;
    };

    class PlayerInputSystem : public engine::ecs::System
    {
    public:
        explicit PlayerInputSystem(engine::Game& game) : System(game)
        {
        };
        void update(const entt::entity& player);

    private:
        static b2Vec2 calculateJumpImpulse(b2BodyId body, const JumpConfig& config);
        void applyJumpImpulse(b2BodyId body) const;
        bool m_jumping = false;
        float distancePerBeat = 2.0f; // Example: player travels 1 unit per beat
    };
} // gl3
