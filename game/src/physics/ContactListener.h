#pragma once
#include "box2d/box2d.h"
#include "../entities/Entity.h"

namespace gl3 {
    class ContactListener {
    public:
        static void checkForCollision(b2WorldId world);

    private:
        static void* getUserDataOfShape(const b2ShapeId& shape);
    };
}