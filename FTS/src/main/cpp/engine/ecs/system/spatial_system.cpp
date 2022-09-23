#include <include/math/xr_linear.h>
#include "ecs/system/spatial_system.h"
#include "ecs/component/types/tracked_space.h"
#include "ecs/component/types/spatial.h"
#include "ecs/ecs.h"

namespace rvr {
void SpatialSystem::UpdateTrackedSpaces(const TrackedSpaceLocations& trackedSpaceLocations) {
    auto trackedSpacePool = ECS::GetInstance()->GetComponentPoolManager()->GetPool(ComponentType::TrackedSpace);
    auto spatialPool = ECS::GetInstance()->GetComponentPoolManager()->GetPool(ComponentType::Spatial);
    for (type::EntityId eid = 0; eid < constants::MAX_ENTITIES; eid++) {
        auto trackedSpace = dynamic_cast<TrackedSpace*>(trackedSpacePool->GetComponent(eid));
        auto spatial = dynamic_cast<Spatial*>(spatialPool->GetComponent(eid));
        if ((!trackedSpace) || (!spatial))
            continue;
        switch (trackedSpace->trackedSpaceType) {
            case TrackedSpaceType::VROrigin:
                spatial->worldPose = trackedSpaceLocations.vrOrigin.pose;
                break;
            case TrackedSpaceType::LeftHand:
                spatial->worldPose = trackedSpaceLocations.leftHand.pose;
                break;
            case TrackedSpaceType::RightHand:
                spatial->worldPose = trackedSpaceLocations.rightHand.pose;
                break;
            default:
                break;
        }
    }
}

void SpatialSystem::CalculateWorldPosition(Entity* entity) {
    if (entity->HasComponent(ComponentType::TrackedSpace) || (entity->id == constants::ROOT_ID))
        return;

    auto parent = entity->GetParent();
    CalculateWorldPosition(parent);
    auto childSpatial = ECS::GetInstance()->GetComponent<Spatial>(entity);
    auto parentSpatial = ECS::GetInstance()->GetComponent<Spatial>(parent);

    // Calculate position based on parent
    XrQuaternionf_Multiply(&childSpatial->worldPose.orientation, &childSpatial->pose.orientation,
                           &parentSpatial->worldPose.orientation);
    XrVector3f offset = XrQuaternionf_Rotate(parentSpatial->worldPose.orientation, childSpatial->pose.position);
    XrVector3f_Add(&childSpatial->worldPose.position, &offset, &parentSpatial->worldPose.position);

    // Place objects relative to the origin
    if (parent->HasComponent(ComponentType::TrackedSpace)) {
        auto ts = ECS::GetInstance()->GetComponent<TrackedSpace>(parent);
        if (ts->trackedSpaceType != TrackedSpaceType::VROrigin)
            XrVector3f_Sub(&childSpatial->worldPose.position, &childSpatial->worldPose.position,
                           &parentSpatial->pose.position);
    }
}
}
