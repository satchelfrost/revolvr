#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/spatial.h"

namespace rvr {
class RenderSystem {
public:
    static std::vector<Spatial*> GetRenderSpatials();
};
}
