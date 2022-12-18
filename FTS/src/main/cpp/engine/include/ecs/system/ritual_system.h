#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/ritual.h"

namespace rvr {
class RitualSystem {
public:
    static void Update(float dt);
    static void Begin();
};
}

