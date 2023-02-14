#pragma once

#include <xr_context.h>
#include "xr_app_helpers.h"
#include "ecs/entity/entity.h"

namespace rvr::system::spatial {
void UpdateTrackedSpaces(XrContext* context);
void UpdateSpatials();
void CalculateWorldPosition(type::EntityId id);
}