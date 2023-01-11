#include <include/math/xr_linear.h>
#include "ecs/system/spatial_system.h"
#include "ecs/component/types/tracked_space.h"
#include "ecs/component/types/spatial.h"
#include "ecs/ecs.h"
#include "pch.h"

namespace rvr {
void SpatialSystem::UpdateTrackedSpaces(const TrackedSpaceLocations& trackedSpaceLocations) {
    for (auto entityId : ECS::Instance()->GetEids(ComponentType::TrackedSpace)) {
        auto [spatial, tracked] = ECS::Instance()->GetComponentPair<Spatial, TrackedSpace>(entityId);
        switch (tracked->type) {
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

void SpatialSystem::CalculateWorldPosition(type::EntityId id) {
    Entity* child = ECS::Instance()->GetEntity(id);
    if (child->HasComponent(ComponentType::TrackedSpace) || (id == constants::ROOT_ID))
        return;

    auto parent = child->GetParent();
    CalculateWorldPosition(parent->id);
    auto childSpatial = ECS::Instance()->GetComponent<Spatial>(child->id);
    auto parentSpatial = ECS::Instance()->GetComponent<Spatial>(parent->id);

    // Calculate position based on parent
    XrQuaternionf_Multiply(&childSpatial->worldPose.orientation, &childSpatial->pose.orientation,
                           &parentSpatial->worldPose.orientation);
    XrVector3f offset = XrQuaternionf_Rotate(parentSpatial->worldPose.orientation, childSpatial->pose.position);
    XrVector3f_Add(&childSpatial->worldPose.position, &offset, &parentSpatial->worldPose.position);

    // Place objects relative to the origin
    if (parent->HasComponent(ComponentType::TrackedSpace)) {
        auto ts = ECS::Instance()->GetComponent<TrackedSpace>(parent->id);
        if (ts->type == TrackedSpaceType::VROrigin) {
            XrVector3f_Sub(&childSpatial->worldPose.position, &childSpatial->worldPose.position,
                           &parentSpatial->pose.position);
        }
    }
}

void SpatialSystem::UpdateSpatials() {
    auto components = ECS::Instance()->GetComponents(ComponentType::Spatial);
    for (auto [eid, component] : components)
        CalculateWorldPosition(component->id);
}
}
