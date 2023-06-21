#pragma once

#include <xr_context.h>
#include "xr_app_helpers.h"
#include "ecs/entity/entity.h"
#include <ecs/component/aux/tracked_space_type.h>

namespace rvr::system::spatial {
void UpdateTrackedSpaces(XrContext* context);
void UpdateSpatials();
void SetSpatialWithJointPose(Spatial* spatial, TrackedSpaceType trackedSpaceType);
math::Transform GetPlayerRelativeTransform(Spatial* pSpatial);
}
