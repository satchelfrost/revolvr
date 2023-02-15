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
                spatial->world = math::Transform(trackedSpaceLocations.vrOrigin.pose, spatial->world.scale);
                break;
            case TrackedSpaceType::LeftHand:
                spatial->world = math::Transform(trackedSpaceLocations.leftHand.pose, spatial->world.scale);
                break;
            case TrackedSpaceType::RightHand:
                spatial->world = math::Transform(trackedSpaceLocations.rightHand.pose, spatial->world.scale);
                break;
            default:
                break;
        }
    }
}

void UpdateWorldTransform(type::EntityId id) {
    Entity* child = ECS::Instance()->GetEntity(id);
    if (id == constants::ROOT_ID)
        return;

    auto parent = child->GetParent();
    UpdateWorldTransform(parent->id);
    auto childSpatial = ECS::Instance()->GetComponent<Spatial>(child->id);
    auto parentSpatial = ECS::Instance()->GetComponent<Spatial>(parent->id);

    if (child->HasComponent(ComponentType::TrackedSpace)) {
        childSpatial->world.scale = childSpatial->local.scale * parentSpatial->world.scale;
    }
    else {
        // Calculate position based on parent
        childSpatial->world.pose.orientation = childSpatial->local.GetOrientation()
                                               * parentSpatial->world.GetOrientation();

        glm::vec3 offset = glm::rotate(parentSpatial->world.GetOrientation(),
                                       childSpatial->local.GetPosition());

        childSpatial->world.pose.position = offset + parentSpatial->world.GetPosition();
        childSpatial->world.scale = childSpatial->local.scale * parentSpatial->world.scale;

        // Place objects relative to the origin
        if (parent->HasComponent(ComponentType::TrackedSpace)) {
            auto ts = ECS::Instance()->GetComponent<TrackedSpace>(parent->id);
            if (ts->type == TrackedSpaceType::VROrigin) {
                childSpatial->world.pose.position = childSpatial->world.GetPosition()
                                                    - parentSpatial->local.GetPosition();
            }
        }
    }
}

void UpdateSpatials() {
    auto components = ECS::Instance()->GetComponents(ComponentType::Spatial);
    for (auto [eid, component] : components)
        UpdateWorldTransform(component->id);
}
}
