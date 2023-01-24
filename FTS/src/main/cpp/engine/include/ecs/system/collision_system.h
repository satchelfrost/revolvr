#pragma once

#include <ecs/component/types/collider.h>

namespace rvr {
class CollisionSystem {
public:
    static void RunCollisionChecks();
};
}
