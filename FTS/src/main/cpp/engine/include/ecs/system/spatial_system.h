#pragma once

#include <xr_context.h>
#include "xr_app_helpers.h"
#include "ecs/entity/entity.h"

namespace rvr {
class SpatialSystem {
public:
    static void UpdateTrackedSpaces(XrContext* context);
    static void UpdateSpatials();
    static void CalculateWorldPosition(type::EntityId id);
};
}