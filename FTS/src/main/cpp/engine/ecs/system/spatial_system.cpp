#include <pch.h>
#include <ecs/ecs.h>
#include <ecs/system/spatial_system.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/tracked_space.h>

namespace rvr::system::spatial{
void UpdateTrackedSpaces(XrContext *context) {
    // First refresh the tracked spaces
    context->RefreshTrackedSpaceLocations();
    auto trackedSpaceLocations = context->trackedSpaceLocations;

    // Update each spatial that has a corresponding tracked space
    for (auto entityId : ECS::Instance()->GetEids(ComponentType::TrackedSpace)) {
        auto [spatial, tracked] = ECS::Instance()->GetComponentPair<Spatial, TrackedSpace>(entityId);
        switch (tracked->type) {
            case TrackedSpaceType::VROrigin:
                spatial->SetWorld(math::Transform(trackedSpaceLocations.vrOrigin.pose,
                                                  spatial->GetWorld().GetScale()));
                break;
            case TrackedSpaceType::LeftHand:
                spatial->SetWorld(math::Transform(trackedSpaceLocations.leftHand.pose,
                                                  spatial->GetWorld().GetScale()));
                break;
            case TrackedSpaceType::RightHand:
                spatial->SetWorld(math::Transform(trackedSpaceLocations.rightHand.pose,
                                                  spatial->GetWorld().GetScale()));
                break;
            default:
                break;
        }
    }
}

void UpdateSpatials() {
    auto components = ECS::Instance()->GetComponents(ComponentType::Spatial);
    for (auto [eid, component] : components)
        dynamic_cast<Spatial *>(component)->UpdateWorld();
}
}
