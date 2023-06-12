#pragma once

#include <xr_context.h>
#include "xr_app_helpers.h"
#include "ecs/entity/entity.h"
#include <ecs/component/aux/tracked_space_type.h>

namespace rvr::system::spatial {
void UpdateTrackedSpaces(XrContext* context);
void UpdateSpatials();
void SetSpatialPose(Spatial* spatial, XrPosef pose);
void HandleJointPose(Spatial* spatial, TrackedSpaceType trackedSpaceType);
}
