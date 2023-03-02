#include <ecs/ecs.h>
#include <ecs/component/types/spatial.h>
#include <ecs/component/types/tracked_space.h>
#include <math/linear_math.h>

namespace rvr {
Spatial::Spatial(type::EntityId pId)
            : Component(ComponentType::Spatial, pId),
              local(math::Transform::Identity()),
              world(math::Transform::Identity()) {}

Spatial::Spatial(type::EntityId pId, const glm::vec3& position,
                 const glm::quat& orientation, const glm::vec3& scale)
            : Component(ComponentType::Spatial, pId),
              local(position, orientation, scale),
              world(math::Transform::Identity()) {}

math::Transform Spatial::GetLocal() {
    return local;
}

void Spatial::SetLocal(const math::Transform& value) {
    local = value;
}

void Spatial::SetLocalPose(const math::Pose& pose) {
    local.SetPose(pose);
}

void Spatial::SetLocalScale(const glm::vec3& scale) {
    local.SetScale(scale);
}

void Spatial::SetLocalPosition(const glm::vec3& position) {
    auto localPose = local.GetPose();
    localPose.SetPosition(position);
    local.SetPose(localPose);
}

void Spatial::SetLocalOrientation(const glm::quat& orientation) {
    auto localPose = local.GetPose();
    localPose.SetOrientation(orientation);
    local.SetPose(localPose);
}

math::Transform Spatial::GetWorld() {
    UpdateWorld();
    return world;
}

void Spatial::SetWorld(const math::Transform& value) {
    world = value;
}

void Spatial::SetWorldPose(const math::Pose &pose) {
    world.SetPose(pose);
}

void Spatial::SetWorldScale(const glm::vec3& scale) {
    world.SetScale(scale);
}

void Spatial::SetWorldPosition(const glm::vec3& position) {
    auto worldPose = world.GetPose();
    worldPose.SetPosition(position);
    world.SetPose(worldPose);
}

void Spatial::SetWorldOrientation(const glm::quat& orientation) {
    auto worldPose = world.GetPose();
    worldPose.SetOrientation(orientation);
    world.SetPose(worldPose);
}

void Spatial::UpdateWorld() {
    Entity* child = ECS::Instance()->GetEntity(id);
    if (id == constants::ROOT_ID)
        return;

    auto parent = child->GetParent();
    auto parentSpatial = ECS::Instance()->GetComponent<Spatial>(parent->id);
    parentSpatial->UpdateWorld();

    if (child->HasComponent(ComponentType::TrackedSpace)) {
        world.SetScale(local.GetScale() * parentSpatial->world.GetScale());
    }
    else {
        // Calculate position based on parent
        world.SetOrientation(parentSpatial->world.GetOrientation() * local.GetOrientation());
        glm::vec3 offset = glm::rotate(parentSpatial->world.GetOrientation(), local.GetPosition());
        world.SetPosition(offset + parentSpatial->world.GetPosition());
        world.SetScale(local.GetScale() * parentSpatial->world.GetScale());

        // Place objects relative to the origin
        if (parent->HasComponent(ComponentType::TrackedSpace)) {
            auto ts = ECS::Instance()->GetComponent<TrackedSpace>(parent->id);
            if (ts->type == TrackedSpaceType::VROrigin) {
                world.SetPosition(world.GetPosition() - parentSpatial->local.GetPosition());
            }
        }
    }
}
}
