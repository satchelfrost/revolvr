#pragma once

#include "ecs/entity/entity.h"
#include "ecs/component/types/spatial.h"

namespace rvr::system::render{
std::vector<Spatial*> GetRenderSpatials();
}
