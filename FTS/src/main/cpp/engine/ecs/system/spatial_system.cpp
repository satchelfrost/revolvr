#include "ecs/system/spatial_system.h"
#include "ecs/component/types/tracked_space.h"

namespace rvr {
SpatialSystem::SpatialSystem() {

}

void SpatialSystem::UpdateTrackedSpaces(const TrackedSpaceLocations& trackedSpaceLocations) {
    auto trackedSpacePool = componentPoolManager_->GetPool(ComponentType::TrackedSpace);
    auto spatialPool = componentPoolManager_->GetPool(ComponentType::Spatial);
    for (type::EntityId eid = 0; eid < constants::MAX_ENTITIES; eid++) {
        auto trackedSpace = dynamic_cast<TrackedSpace*>(trackedSpacePool->GetComponent(eid));
        auto spatial = dynamic_cast<Spatial*>(spatialPool->GetComponent(eid));
        switch (trackedSpace->trackedSpaceType) {
            case TrackedSpaceType::VROrigin:
                spatial->pose = trackedSpaceLocations.vrOrigin.pose;
                break;
            case TrackedSpaceType::LeftHand:
                spatial->pose = trackedSpaceLocations.leftHand.pose;
                break;
            case TrackedSpaceType::RightHand:
                spatial->pose = trackedSpaceLocations.rightHand.pose;
                break;
            default:
                break;
        }
    }
}

void SpatialSystem::CalculateWorldPosition(Entity* entity) {
//     parent = entity->GetParent();
//     if (entity->HasComponent(ComponentType::TrackedSpace) || (entity->id == constants::ROOT_ID))
//         return;
//     CalculateWorldPosition(parent)
//     auto spatial = componentPoolManager_->GetComponent<Spatial>(entity);
//     auto parentSpatial = componentPoolManager_->GetComponent<Spatial>(parent);
//     spatial = parentSpatial * spatial
}
}
