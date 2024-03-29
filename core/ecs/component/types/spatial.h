/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

#pragma once

#include <ecs/component/component.h>
#include <math/transform.h>
#include "ecs/entity/entity.h"

namespace rvr {
class Spatial : public Component {
public:

    Spatial(type::EntityId pId);
    Spatial(type::EntityId pId, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);
    Spatial(type::EntityId pId, math::Transform local, math::Transform world);

    Spatial(const Spatial& other) = delete;
    Spatial(const Spatial& other, type::EntityId pId);
    virtual Component* Clone(type::EntityId newEntityId) override;

    math::Transform GetLocal();
    void SetLocal(const math::Transform& value);
    void SetLocalPose(const math::Pose& pose);
    void SetLocalScale(const glm::vec3& scale);
    void SetLocalPosition(const glm::vec3& position);
    void SetLocalOrientation(const glm::quat& orientation);

    math::Transform GetWorld();
    void SetWorld(const math::Transform& value);
    void SetWorldPose(const math::Pose& pose);
    void SetWorldScale(const glm::vec3& scale);
    void SetWorldPosition(const glm::vec3& position);
    void SetWorldOrientation(const glm::quat& orientation);

    void UpdateWorld();
    void MakeStaleRecursive();

    // Sets the local transform relative to another spatial
    void SetLocalRelative(Spatial* other);

private:
    void ApplyParentRTS(Spatial* parentSpatial);
    math::Transform local_;
    math::Transform world_;
    bool stale_;

};
}