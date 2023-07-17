/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

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
              local_(math::Transform::Identity()),
              world_(math::Transform::Identity()),
              stale_(true){}

Spatial::Spatial(type::EntityId pId, const glm::vec3& position,
                 const glm::quat& orientation, const glm::vec3& scale)
            : Component(ComponentType::Spatial, pId),
              local_(position, orientation, scale),
              world_(math::Transform::Identity()),
              stale_(true){}

Spatial::Spatial(type::EntityId pId, math::Transform local, math::Transform world) :
Component(ComponentType::Spatial, pId), world_(world), local_(local), stale_(true){}

math::Transform Spatial::GetLocal() {
    return local_;
}

void Spatial::SetLocal(const math::Transform& value) {
    local_ = value;
    MakeStaleRecursive();
}

void Spatial::SetLocalPose(const math::Pose& pose) {
    local_.SetPose(pose);
    MakeStaleRecursive();
}

void Spatial::SetLocalScale(const glm::vec3& scale) {
    local_.SetScale(scale);
    MakeStaleRecursive();
}

void Spatial::SetLocalPosition(const glm::vec3& position) {
    auto localPose = local_.GetPose();
    localPose.SetPosition(position);
    local_.SetPose(localPose);
    MakeStaleRecursive();
}

void Spatial::SetLocalOrientation(const glm::quat& orientation) {
    auto localPose = local_.GetPose();
    localPose.SetOrientation(orientation);
    local_.SetPose(localPose);
    MakeStaleRecursive();
}

math::Transform Spatial::GetWorld() {
    UpdateWorld();
    return world_;
}

void Spatial::SetWorld(const math::Transform& value) {
    world_ = value;
}

void Spatial::SetWorldPose(const math::Pose &pose) {
    world_.SetPose(pose);
}

void Spatial::SetWorldScale(const glm::vec3& scale) {
    world_.SetScale(scale);
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

    auto localPose = local_.GetPose();
    localPose.SetPosition(localPosition);
    local_.SetPose(localPose);
}

void Spatial::SetWorldOrientation(const glm::quat& orientation) {
    auto worldPose = world_.GetPose();
    worldPose.SetOrientation(orientation);
    world_.SetPose(worldPose);
}

void Spatial::UpdateWorld() {
    if (!stale_)
        return;

    Entity* thisEntity = GetEntity(id);
    auto parentEntity = thisEntity->GetParent();
    if (parentEntity) {
        auto parentSpatial = GetComponent<Spatial>(parentEntity->id);
        if (parentSpatial) {
            parentSpatial->UpdateWorld();
            ApplyParentRTS(parentSpatial);
        }
    }
    else {
        world_ = local_;
    }

    stale_ = false;
}

Component *Spatial::Clone(type::EntityId newEntityId) {
    return new Spatial(*this, newEntityId);
}

Spatial::Spatial(const Spatial& other, type::EntityId newEntityId) :
Component(ComponentType::Spatial, newEntityId), local_(other.local_), world_(other.world_) {}


void Spatial::ApplyParentRTS(Spatial* parentSpatial) {
    // Update the world transform
    math::Transform parentWorld = parentSpatial->world_;
    world_.SetOrientation(parentWorld.GetOrientation() * local_.GetOrientation());
    world_.SetPosition(parentWorld.GetPosition() + (parentWorld.GetOrientation() * local_.GetPosition()));
    world_.SetScale(local_.GetScale() * parentWorld.GetScale());
}

void Spatial::MakeStaleRecursive() {
    stale_ = true;
    Entity* thisEntity = GetEntity(id);
    for (auto child : thisEntity->GetChildren()) {
        auto spatial = GetComponent<Spatial>(child->id);
        if (spatial)
            spatial->MakeStaleRecursive();
    }
}

void Spatial::SetLocalRelative(Spatial *other) {
    math::Transform relTransform(math::Transform::Identity());
    math::Transform otherWorld = other->GetWorld();
    math::Transform thisWorld = GetWorld();

    auto relativePosition = thisWorld.GetPosition() - otherWorld.GetPosition();
    auto invOrientation = glm::inverse(otherWorld.GetOrientation());
    relTransform.SetPosition(invOrientation * relativePosition);
    relTransform.SetOrientation(invOrientation * thisWorld.GetOrientation());
    relTransform.SetScale(thisWorld.GetScale() / otherWorld.GetScale());
    local_ = relTransform;
}
}