#pragma once
#include "ecs.h"

namespace gl3::engine::ecs {
    class Component {
        friend class ComponentManager;
        friend class Entity;

    public:
        virtual ~Component() = default;

        [[nodiscard]] guid_t entity() const { return owner; }
        [[nodiscard]] bool isDeleted() const { return deleted; }

    protected:
        explicit Component(guid_t owner = invalidID)
                : owner(owner) {};

        guid_t owner;
        bool deleted = false;
    };
}
