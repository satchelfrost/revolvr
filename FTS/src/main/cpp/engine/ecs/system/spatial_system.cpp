#include <include/math/xr_linear.h>
#include <ecs/system/spatial_system.h>
#include <ecs/component/types/tracked_space.h>
#include <ecs/component/types/spatial.h>
#include <ecs/ecs.h>
#include <pch.h>

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
                spatial->worldTransform = math::Transform(trackedSpaceLocations.vrOrigin.pose);
                break;
            case TrackedSpaceType::LeftHand:
                spatial->worldTransform = math::Transform(trackedSpaceLocations.leftHand.pose);
                break;
            case TrackedSpaceType::RightHand:
                spatial->worldTransform = math::Transform(trackedSpaceLocations.rightHand.pose);
                break;
            default:
                break;
        }
    }
}

void CalculateWorldPosition(type::EntityId id) {
    Entity* child = ECS::Instance()->GetEntity(id);
    if (child->HasComponent(ComponentType::TrackedSpace) || (id == constants::ROOT_ID))
        return;

    auto parent = child->GetParent();
    CalculateWorldPosition(parent->id);
    auto childSpatial = ECS::Instance()->GetComponent<Spatial>(child->id);
    auto parentSpatial = ECS::Instance()->GetComponent<Spatial>(parent->id);

    // Calculate position based on parent
//    XrQuaternionf_Multiply(&childSpatial->worldPose.orientation, &childSpatial->pose.orientation,
//                           &parentSpatial->worldPose.orientation);
    childSpatial->worldTransform.SetOrientation(childSpatial->transform.GetOrientation()
                                                * parentSpatial->worldTransform.GetOrientation());
//    XrVector3f offset = XrQuaternionf_Rotate(parentSpatial->worldPose.orientation, childSpatial->pose.position);
    glm::vec3 offset = glm::rotate(parentSpatial->worldTransform.GetOrientation(),
                                   childSpatial->transform.GetPosition());
//    XrVector3f_Add(&childSpatial->worldPose.position, &offset, &parentSpatial->worldPose.position);
    childSpatial->worldTransform.SetPosition(offset + parentSpatial->worldTransform.GetPosition());

    // Place objects relative to the origin
    if (parent->HasComponent(ComponentType::TrackedSpace)) {
        auto ts = ECS::Instance()->GetComponent<TrackedSpace>(parent->id);
        if (ts->type == TrackedSpaceType::VROrigin) {
//            XrVector3f_Sub(&childSpatial->worldPose.position, &childSpatial->worldPose.position,
//                           &parentSpatial->pose.position);
            childSpatial->worldTransform.SetPosition(childSpatial->worldTransform.GetPosition()
                                                     - parentSpatial->transform.GetPosition());
        }
    }
}

void UpdateSpatials() {
    auto components = ECS::Instance()->GetComponents(ComponentType::Spatial);
    for (auto [eid, component] : components)
        CalculateWorldPosition(component->id);
}
}
