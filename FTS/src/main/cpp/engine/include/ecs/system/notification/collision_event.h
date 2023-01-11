#pragma once

#include "event.h"
#include <ecs/component/types/collider.h>

namespace rvr {
class Collider;

class CollisionEvent : public Event {
public:
    CollisionEvent(Collider* src, Collider* other);
    Collider* src;
    Collider* other;
};
}