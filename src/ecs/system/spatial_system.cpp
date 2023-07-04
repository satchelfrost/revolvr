/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <pch.h>
#include <ecs/system/spatial_system.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/tracked_space.h>
#include <global_context.h>

#define GetComponentPair GlobalContext::Inst()->GetECS()->GetComponentPair
#define LeftHandTracker GlobalContext::Inst()->GetXrContext()->handTrackerLeft_
#define RightHandTracker GlobalContext::Inst()->GetXrContext()->handTrackerRight_

namespace rvr::system::spatial{
void UpdateTrackedSpaces(XrContext *context) {
    // First refresh the tracked spaces
    context->RefreshTrackedSpaceLocations();
    auto trackedSpaceLocations = context->trackedSpaceLocations;

    // Update each spatial that has a corresponding tracked space
    for (auto entityId : GlobalContext::Inst()->GetECS()->GetEids(ComponentType::TrackedSpace)) {
        auto [spatial, tracked] = GetComponentPair<Spatial, TrackedSpace>(entityId);
        switch (tracked->type) {
            case TrackedSpaceType::LeftController:
                spatial->SetLocal(math::Transform(trackedSpaceLocations.leftHand.pose,
                                                  spatial->GetLocal().GetScale()));
                break;
            case TrackedSpaceType::RightController:
                spatial->SetLocal(math::Transform(trackedSpaceLocations.rightHand.pose,
                                                  spatial->GetLocal().GetScale()));
                break;
            case TrackedSpaceType::Head:
                spatial->SetLocal(math::Transform(trackedSpaceLocations.head.pose,
                                                  spatial->GetLocal().GetScale()));
                break;
            default:
                int tsType = (int)tracked->type;
                int lowerBound = (int)TrackedSpaceType::LeftCenterJoint;
                int upperBound = (int)TrackedSpaceType::RightPinkyTip;
                if (tsType >= lowerBound && tsType <= upperBound)
                    SetSpatialWithJointPose(spatial, tracked->type);
                break;
        }
    }
}

void UpdateSpatials() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Spatial);
    for (auto [eid, component] : components)
        reinterpret_cast<Spatial*>(component)->UpdateWorld();
}

void SetSpatialWithJointPose(Spatial* spatial, TrackedSpaceType trackedSpaceType) {
    // Map the TrackedSpaceType to integer from 0 -> 25 i.e. the 26 possible joints
    int joint = (int)trackedSpaceType;
    int rightHandOffset = (int)TrackedSpaceType::RightCenterJoint;
    int leftHandOffset = (int)TrackedSpaceType::LeftCenterJoint;
    if (joint < rightHandOffset) {
        joint = joint - leftHandOffset;
        LeftHandTracker.SetSpatialWithValidJointPose(joint, spatial);
    }
    else {
        joint = joint - rightHandOffset;
        RightHandTracker.SetSpatialWithValidJointPose(joint, spatial);
    }
}

math::Transform GetPlayerRelativeTransform(Spatial* spatial) {
    // Get the player and spatial world transforms
    math::Transform playerRelTransform(math::Transform::Identity());
    auto player = GlobalContext::Inst()->GetECS()->GetComponent<Spatial>(GlobalContext::Inst()->PLAYER_ID);
    CHECK_MSG(player, "Player spatial does not exist inside of GetPlayerRelativeTransform()");
    math::Transform playerWorld = player->GetWorld();
    math::Transform spatialWorld = spatial->GetWorld();

    // Calculate player relative transform
    auto relativePosition = spatialWorld.GetPosition() - playerWorld.GetPosition();
    auto invOrientation = glm::inverse(playerWorld.GetOrientation());
    playerRelTransform.SetPosition(invOrientation * relativePosition);
    playerRelTransform.SetOrientation(invOrientation * spatialWorld.GetOrientation());
    playerRelTransform.SetScale(spatialWorld.GetScale() * playerWorld.GetScale());
    return playerRelTransform;
}
}
