#include <pch.h>
#include <ecs/system/spatial_system.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/tracked_space.h>
#include <global_context.h>

#define GetComponentPair GlobalContext::Inst()->GetECS()->GetComponentPair

namespace rvr::system::spatial{

void UpdateTrackedSpaces(XrContext *context) {
    // First refresh the tracked spaces
    context->RefreshTrackedSpaceLocations();
    auto trackedSpaceLocations = context->trackedSpaceLocations;

    // Update each spatial that has a corresponding tracked space
    for (auto entityId : GlobalContext::Inst()->GetECS()->GetEids(ComponentType::TrackedSpace)) {
        auto [spatial, tracked] = GetComponentPair<Spatial, TrackedSpace>(entityId);
        switch (tracked->type) {
            case TrackedSpaceType::Player:
                SetSpatialPose(spatial, trackedSpaceLocations.vrOrigin.pose);
                break;
            case TrackedSpaceType::LeftController:
                SetSpatialPose(spatial, trackedSpaceLocations.leftHand.pose);
                break;
            case TrackedSpaceType::RightController:
                SetSpatialPose(spatial, trackedSpaceLocations.rightHand.pose);
                break;
            case TrackedSpaceType::Head:
                SetSpatialPose(spatial, trackedSpaceLocations.head.pose);
                break;
            default:
                int tsType = (int)tracked->type;
                int lowerBound = (int)TrackedSpaceType::LeftHandJoint0;
                int upperBound = (int)TrackedSpaceType::RightPinkyTip;
                if (tsType >= lowerBound && tsType <= upperBound)
                    HandleJointPose(spatial, tracked->type);
                break;
        }
    }
}

void UpdateSpatials() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Spatial);
    for (auto [eid, component] : components)
        dynamic_cast<Spatial *>(component)->UpdateWorld();
}

void SetSpatialPose(Spatial* spatial, XrPosef pose){
    spatial->SetWorld(math::Transform(pose,spatial->GetWorld().GetScale()));
}

void HandleJointPose(Spatial* spatial, TrackedSpaceType trackedSpaceType) {
    // Find out which hand and calculate joint offset by mapping enum to integer from 0 -> 25
    bool usingLeft = false;
    int offset = (int)TrackedSpaceType::RightHandJoint0;
    int joint = (int)trackedSpaceType;
    if (joint < offset) {
        usingLeft = true;
        offset = (int)TrackedSpaceType::LeftHandJoint0;
    }
    joint = joint - offset;

    math::Transform transform;
    bool jointValid;
    if (usingLeft)
        jointValid = GlobalContext::Inst()->GetXrContext()->handTrackerLeft_.GetValidJointLocation(joint, transform);
    else
        jointValid = GlobalContext::Inst()->GetXrContext()->handTrackerRight_.GetValidJointLocation(joint, transform);
    if (jointValid) {
        transform.SetScale(spatial->GetWorld().GetScale());
        spatial->SetWorld(transform);
    }
}
}
