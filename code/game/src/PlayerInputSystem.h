#pragma once
#include "box2d/box2d.h"
#include "engine/Game.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"
#include "engine/ecs/System.h"

namespace gl3::game::input
{
    class PlayerInputSystem : public engine::ecs::System
    {
    public:
        explicit PlayerInputSystem(engine::Game& game) : System(game)
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::LevelLengthComputed>().connect<&
                PlayerInputSystem::onLvlLengthCompute>(this);
        };

        ~PlayerInputSystem() override
        {
            engine::ecs::EventDispatcher::dispatcher.sink<engine::ecs::LevelLengthComputed>().disconnect<&
                PlayerInputSystem::onLvlLengthCompute>(this);
        };
        void update();

    private:
        void onLvlLengthCompute(const engine::ecs::LevelLengthComputed& event);
        void applyJumpImpulse(b2BodyId body);
        float curr_lvl_speed = 1.f;
        bool enter_pressed_ = false;
        bool canJump = true;
        float desiredJumpHeight = 1.f;
        float jumpHeightFactor = 1.f;
        float landingUnitsAhead = 1.f;
        entt::entity player_ = entt::null;
        float rotationSpeed = -270.f;
    };
} // gl3
