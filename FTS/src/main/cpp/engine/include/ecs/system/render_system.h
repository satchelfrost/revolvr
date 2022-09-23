#pragma once

#include "ecs/entity/entity.h"

namespace rvr {
class RenderSystem {
public:
    static std::vector<Entity*> GatherRenderables();
};
}
