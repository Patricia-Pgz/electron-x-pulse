#include "ContactListener.h"

#include <iostream>

namespace gl3 {

    void ContactListener::checkForCollision(b2WorldId world) {
        b2ContactEvents contactEvents = b2World_GetContactEvents(world);
        std::cout << "Contact Event Count: " << contactEvents.beginCount << std::endl;
        for (int i = 0; i < contactEvents.beginCount; ++i) {
            b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
            const auto bodyUserDataA = getUserDataOfShape(beginEvent->shapeIdA);
            const auto bodyUserDataB = getUserDataOfShape(beginEvent->shapeIdB);
            if(bodyUserDataA == nullptr) std::cout << "nodata";
            if (bodyUserDataA) {
                std::cout << "colwithA";
                auto entity = static_cast<Entity*>(bodyUserDataA);
                entity->startContact();
            }

            if (bodyUserDataB) {
                std::cout << "colwithB";
                auto entity = static_cast<Entity*>(bodyUserDataB);
                entity->startContact();
            }
        }
    }

    void* ContactListener::getUserDataOfShape(const b2ShapeId& shape) {
        return b2Body_GetUserData(b2Shape_GetBody(shape));
    }

}
