#pragma once

#include "xr_app_helpers.h"
#include "ecs/component/component_pool_manager.h"
#include "ecs/component/types/spatial.h"
//#include "ecs/entity/entity.h"

namespace rvr {
class SpatialSystem {
public:
    SpatialSystem();
    void UpdateTrackedSpaces(const TrackedSpaceLocations& trackedSpaceLocations);
    void CalculateWorldPosition(Entity* entity);
private:
    ComponentPoolManager* componentPoolManager_;
};
}