#include <ecs/component/types/spatial.h>
#include <ecs/component/types/tracked_space.h>
#include <math/linear_math.h>
#include <global_context.h>

#define GetComponent GlobalContext::Inst()->GetECS()->GetComponent
#define GetEntity GlobalContext::Inst()->GetECS()->GetEntity
#define GetParentSpatial(pId) GetComponent<Spatial>(GetEntity(pId)->GetParent()->id)

namespace rvr {
Spatial::Spatial(type::EntityId pId)
            : Component(ComponentType::Spatial, pId),
              local(math::Transform::Identity()),
              world(math::Transform::Identity()),
              plart(math::Transform::Identity()){}

Spatial::Spatial(type::EntityId pId, const glm::vec3& position,
                 const glm::quat& orientation, const glm::vec3& scale)
            : Component(ComponentType::Spatial, pId),
              local(position, orientation, scale),
              world(math::Transform::Identity()),
              plart(math::Transform::Identity()){}

Spatial::Spatial(type::EntityId pId, math::Transform local, math::Transform world) :
Component(ComponentType::Spatial, pId), world(world), local(local), plart(math::Transform::Identity()){}

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
    glm::vec3 localPosition;
    if (id == constants::ROOT_ID) {
        localPosition = position;
    }
    else {
        auto pSpatial = GetParentSpatial(id);
        auto pOrientationInverse = glm::inverse(pSpatial->GetWorld().GetOrientation());
        auto pPosition = pSpatial->GetWorld().GetPosition();

        localPosition = glm::rotate(pOrientationInverse, (pPosition - position));
    }

    auto localPose = local.GetPose();
    localPose.SetPosition(localPosition);
    local.SetPose(localPose);
}

void Spatial::SetWorldOrientation(const glm::quat& orientation) {
    auto worldPose = world.GetPose();
    worldPose.SetOrientation(orientation);
    world.SetPose(worldPose);
}

void Spatial::UpdateWorld() {
    Entity* thisEntity = GetEntity(id);
    if (id == constants::ROOT_ID) {
        // First make sure player global is set with player local
        auto player = GetComponent<Spatial>(1);
        player->SetWorld(player->GetLocal());
        CalculatePlart();
        return;
    }

    if (id == 1)
        return;

    auto parent = thisEntity->GetParent();
    CHECK_MSG(parent, Fmt("Entity %s is an orphan", thisEntity->GetName().c_str()));
    auto parentSpatial = GetComponent<Spatial>(parent->id);
    parentSpatial->UpdateWorld();

    if (thisEntity->HasComponent(ComponentType::TrackedSpace)) {
        world.SetScale(local.GetScale() * parentSpatial->world.GetScale());
        plart = world;
    }
    else {
        ConcantenateRTS(parentSpatial->world, local, world);
        if (parent->HasComponent(ComponentType::TrackedSpace))
            plart = world;
        else
            CalculatePlart();
    }
}

Component *Spatial::Clone(type::EntityId newEntityId) {
    return new Spatial(*this, newEntityId);
}

Spatial::Spatial(const Spatial& other, type::EntityId newEntityId) :
Component(ComponentType::Spatial, newEntityId), local(other.local), world(other.world) {}

void Spatial::ConcantenateRTS(math::Transform from, math::Transform to, math::Transform &result) {
    // Calculate orientation based on parent
    glm::quat orientation = from.GetOrientation() * to.GetOrientation();

    // Rotate the position using the combined orientation
    glm::vec3 rotatedPosition = orientation * to.GetPosition();

    // Update the world transformation
    result.SetOrientation(orientation);
    result.SetPosition(rotatedPosition + from.GetPosition());
    result.SetScale(to.GetScale() * from.GetScale());
}

void Spatial::CalculatePlart() {
    // Relative pose
    auto player = GetComponent<Spatial>(1); // hard code for now
    auto relativePosition = world.GetPosition() - player->world.GetPosition();
    plart.SetPosition(player->world.GetOrientation() * relativePosition);
    plart.SetOrientation(player->world.GetOrientation() * world.GetOrientation());
    plart.SetScale(world.GetScale() * player->world.GetScale());
}

math::Transform Spatial::GetPlart() {
    return plart;
}
}
