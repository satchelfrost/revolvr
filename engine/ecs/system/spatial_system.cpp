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
            case TrackedSpaceType::Head:
                spatial->SetWorld(math::Transform(trackedSpaceLocations.head.pose,
                                                  spatial->GetWorld().GetScale()));
                break;
            default:
                break;
        }
    }
}

void UpdateSpatials() {
    auto components = GlobalContext::Inst()->GetECS()->GetComponents(ComponentType::Spatial);
    for (auto [eid, component] : components)
        dynamic_cast<Spatial *>(component)->UpdateWorld();
}
}
